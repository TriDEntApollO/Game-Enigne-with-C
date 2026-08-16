#ifndef GAME_ENGINE_RENDER_H
#define GAME_ENGINE_RENDER_H

#include <SDL3/SDL.h>
#include <linmath.h>

#include <engine/types.h>

#define MAX_BATCH_QUADS    10000
#define MAX_BATCH_VERTICES 40000
#define MAX_BATCH_ELEMENTS 60000


typedef struct batch_vertex {
    vec2 position;
    vec2 uvs;
    vec4 color;

} BatchVertex;

typedef struct sprite_sheet {
    f32 width;
    f32 height;
    f32 cell_width;
    f32 cell_height;
    u32 texture_id;
} SpriteSheet;


SDL_Window *render_init(void);
void render_begin(void);
void render_end(SDL_Window *window, u32 batch_texture_id);

void render_line_segment(vec2 start, vec2 end, vec4 color);
void render_quad_line(vec2 pos, vec2 size, vec4 color);
void render_quad(vec2 pos, vec2 size, vec4 color);
void render_aabb(f32 *aabb, vec4 color);
f32 render_get_scale(void);

void render_sprite_sheet_init(SpriteSheet *sprite_sheet, const char *path, f32 cell_width, f32 cell_height);
void render_sprite_sheet_frame(SpriteSheet *sprite_sheet, f32 row, f32 column, vec2 position);

#endif // GAME_ENGINE_RENDER_H