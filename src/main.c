#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <glad/gl.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <engine/time.h>
#include <engine/util.h>
#include <engine/input.h>
#include <engine/config.h>
#include <engine/global.h>
#include <engine/render.h>
#include <engine/entity.h>
#include <engine/physics.h>


typedef enum collision_layer {
    COLLISION_LAYER_PLAYER    = 1 << 0,
    COLLISION_LAYER_ENEMY     = 1 << 1,
    COLLISION_LAYER_TERRAIN   = 1 << 2,
} CollisionLayer;


static bool should_quit = false;
static vec4 player_color = { 0.0f, 1.0f, 1.0f, 1.0f };
bool player_is_on_ground = false;

static void input_handle(Body *player_body) {
    if (global.input.escape == KS_PRESSED || global.input.escape == KS_HELD) {
        should_quit = true;
    }

    f32 velx = 0, vely = player_body->velocity[1];

    if (global.input.right == KS_PRESSED || global.input.right == KS_HELD) {
        velx += 600.0f;
    }
    if (global.input.left == KS_PRESSED || global.input.left == KS_HELD) {
        velx -= 600.0f;
    }
    if ((global.input.up == KS_PRESSED || global.input.up == KS_HELD || 
        global.input.space == KS_PRESSED || global.input.space == KS_HELD) && 
        player_is_on_ground) {
        vely = 2000.0f;
        player_is_on_ground = false;
    }
    // if (global.input.down == KS_PRESSED || global.input.down == KS_HELD) {
    //     vely -= 800.0f;
    // }

    player_body->velocity[0] = velx;
    player_body->velocity[1] = vely;
}


static inline SDL_Window *init_engine(void) {
    time_init(60);
    config_init();
    SDL_Window *window = render_init();
    physics_init();
    entity_init();

    return window;
}


void player_on_hit(Body *self, Body *other, Hit hit) {
    if (other->collision_layer == COLLISION_LAYER_ENEMY) {
        player_color[0] = 1.0f;
        player_color[2] = 0.0f;
    }
}

void player_on_hit_static(Body *self, StaticBody *other, Hit hit) {
    if (hit.normal[1] > 0) {
        player_is_on_ground = true;
    }
}

void enemy_on_hit_static (Body *self, StaticBody *other, Hit hit) {
    if (hit.normal[0] > 0) {
        self->velocity[0] = 400.0f;
    }

    if (hit.normal[0] < 0) {
        self->velocity[0] = -400.0f;
    }
}


int main(int argc, char *argv[]) {
    SDL_Window *window = init_engine();
    SDL_HideCursor();

    i32 window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);

    u8 enemy_mask = COLLISION_LAYER_PLAYER | COLLISION_LAYER_TERRAIN;
    u8 player_mask = COLLISION_LAYER_ENEMY | COLLISION_LAYER_TERRAIN;

    usize player_id = entity_create((vec2){ 100, 200 }, (vec2){ 24, 24 }, (vec2){ 0, 0 }, COLLISION_LAYER_PLAYER, player_mask, player_on_hit, player_on_hit_static);
    f32 width = window_width / render_get_scale();
    f32 height = window_height / render_get_scale();

    usize static_body_a_id = physics_static_body_create((vec2){ width * 0.5f - 12.5f, height - 12.5f }, (vec2){ width - 25.0f, 25.0f }, COLLISION_LAYER_TERRAIN); 
    usize static_body_b_id = physics_static_body_create((vec2){ width - 12.5f, height * 0.5f + 12.5f }, (vec2){ 25.0f, height - 25.0f }, COLLISION_LAYER_TERRAIN); 
    usize static_body_c_id = physics_static_body_create((vec2){ width * 0.5f + 12.5f, 12.5f }, (vec2){ width - 25.0f, 25.0f }, COLLISION_LAYER_TERRAIN); 
    usize static_body_d_id = physics_static_body_create((vec2){ 12.5f, height * 0.5f - 12.5f }, (vec2){ 25.0f, height - 25.0f }, COLLISION_LAYER_TERRAIN); 
    usize static_body_e_id = physics_static_body_create((vec2){ width * 0.5f, height * 0.5f }, (vec2){ 62.5f, 62.5f }, COLLISION_LAYER_TERRAIN);

    usize enemy_a_id = entity_create((vec2){ 200, 200 }, (vec2){ 25, 25 }, (vec2){400, 0 }, COLLISION_LAYER_ENEMY, enemy_mask, NULL, enemy_on_hit_static);
    usize enemy_b_id = entity_create((vec2){ 300, 300 }, (vec2){ 25, 25 }, (vec2){ 400, 0 }, COLLISION_LAYER_ENEMY, enemy_mask, NULL, enemy_on_hit_static);
    
    SpriteSheet sprite_sheet_player;
    render_sprite_sheet_init(&sprite_sheet_player, "assets/player.png", 24, 24);

    SDL_Event event;
    while (!should_quit) {
        time_update();
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                case SDL_EVENT_QUIT:
                    printf("info: user requested to close the window\n");
                    should_quit = true;
                    break;
                default:
                    break;
            }
        }

        Entity *player = entity_get_active(player_id);
        Body *player_body = physics_body_get(player->body_id);
        StaticBody *static_body_a = physics_static_body_get(static_body_a_id);
        StaticBody *static_body_b = physics_static_body_get(static_body_b_id);
        StaticBody *static_body_c = physics_static_body_get(static_body_c_id);
        StaticBody *static_body_d = physics_static_body_get(static_body_d_id);
        StaticBody *static_body_e = physics_static_body_get(static_body_e_id);

        input_update();
        input_handle(player_body);
        
        render_begin();
        physics_update();

        render_aabb((f32 *)static_body_a, WHITE);
        render_aabb((f32 *)static_body_b, WHITE);
        render_aabb((f32 *)static_body_c, WHITE);
        render_aabb((f32 *)static_body_d, WHITE);
        render_aabb((f32 *)static_body_e, WHITE);
        render_aabb((f32 *)player_body, player_color);

        render_aabb((f32 *)physics_body_get(entity_get_active(enemy_a_id)->body_id), GRAY);
        render_aabb((f32 *)physics_body_get(entity_get_active(enemy_b_id)->body_id), GRAY);

        render_sprite_sheet_frame(&sprite_sheet_player, 1, 2, (vec2){100, 100});
        render_sprite_sheet_frame(&sprite_sheet_player, 0, 4, (vec2){200, 200});
        render_sprite_sheet_frame(&sprite_sheet_player, 0, 0, player_body->aabb.position);
        
        render_end(window, sprite_sheet_player.texture_id);

        player_color[0] = 0.0f;
        player_color[2] = 1.0f;

        time_update_late();
    }


    // SDL_GL_DestroyContext(global.render.gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
