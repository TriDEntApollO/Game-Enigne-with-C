#ifndef GAME_ENGINE_RENDER_H
#define GAME_ENGINE_RENDER_H

#include <SDL3/SDL.h>
#include <linmath.h>

#include <engine/types.h>


typedef struct {
    f32 width;
    f32 height;
    SDL_Window *window;
    SDL_GLContext gl_context;
} RenderState;

void render_init(void);
void render_beign(void);
void render_end(void);

void render_quad(vec2 pos, vec2 size, vec4 color);

#endif // GAME_ENGINE_RENDER_H