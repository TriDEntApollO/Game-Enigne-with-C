#include <stdio.h>
#include <stdbool.h>
#include <glad/gl.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <engine/time.h>
#include <engine/input.h>
#include <engine/config.h>
#include <engine/global.h>
#include <engine/render.h>

static const GLuint WIDTH = 800, HEIGHT = 600;
static bool should_quit = false;
static vec2 position;

static void input_handle(void) {
    if (global.input.left == KS_PRESSED || global.input.left == KS_HELD)
        position[0] -= 500.0f * global.time.delta;
    if (global.input.right == KS_PRESSED || global.input.right == KS_HELD)
        position[0] += 500.0f * global.time.delta;
    if (global.input.up == KS_PRESSED || global.input.up == KS_HELD)
        position[1] += 500.0f * global.time.delta;
    if (global.input.down == KS_PRESSED || global.input.down == KS_HELD)
        position[1] -= 500.0f * global.time.delta;
    if (global.input.escape == KS_PRESSED || global.input.escape == KS_HELD)
        should_quit = true;
}

int main(int argc, char *argv[]) {
    time_init(60);
    config_init();
    render_init();

    position[0] = global.render.width * 0.5f;
    position[1] = global.render.height * 0.5f;

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

        input_update();
        input_handle();

        render_begin();
        render_quad(position, (vec2){50, 50}, (vec4){0, 1, 0, 1});

        render_end();
        time_update_late();
    }


    SDL_GL_DestroyContext(global.render.gl_context);
    SDL_DestroyWindow(global.render.window);
    SDL_Quit();

    return 0;
}
