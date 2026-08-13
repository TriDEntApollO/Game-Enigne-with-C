#include <glad/gl.h>
#include <SDL3/SDL.h>
#include <engine/util.h>
#include <engine/global.h>

#include "render_internal.h"


const char *DEFAULT_VERTEX_PATH = "./src/engine/shaders/default.vert";
const char *DEFAULT_FRAGMENT_PATH = "./src/engine/shaders/default.frag";

SDL_Window *render_init_window(u32 width, u32 height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        ERROR_EXIT("error: failed to initialize SDL: %s\n", SDL_GetError());
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = SDL_CreateWindow("Game Enigne", width, height, SDL_WINDOW_OPENGL);
    if (!window) {
        const char *error = SDL_GetError();
        SDL_Quit();
        ERROR_EXIT("error: failed to create window: %s\n", error);
    }
    global.render.window = window;

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        const char *error = SDL_GetError();
        SDL_DestroyWindow(window);
        SDL_Quit();
        ERROR_EXIT("error: failed to create OpenGL context: %s\n", error);
    }
    global.render.gl_context = gl_context;

    int version = gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress);
    if (version == 0) {
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        ERROR_EXIT("error: failed to initialize OpenGL loader!\n");
    }

    printf("openGL %d.%d loaded\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));

    return window;
}

void render_init_shaders(RenderStateInternal *state) {
    state->shader_default = render_shader_create(DEFAULT_VERTEX_PATH, DEFAULT_FRAGMENT_PATH);

    mat4x4_ortho(state->projection, 0, global.render.width, 0, global.render.height, -2, 2);
    glUseProgram(state->shader_default);
    glUniformMatrix4fv(
        glGetUniformLocation(state->shader_default, "projection"), 
        1, 
        GL_FALSE, 
        (const GLfloat *)state->projection
    );

}

void render_init_color_texture(u32 *texture) {
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    // Create a 1x1 white pixel texture
    u8 white_pixel[4] = {255, 255, 255, 255}; // RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white_pixel);

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
}

void render_init_quad(u32 *vao, u32 *vbo, u32 *ebo) {
    //   x,   y,   z,   u,   v
    f32 vertices[] = {
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 1.0f, 0.0f
    };

    u32 indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);
    glGenBuffers(1, ebo);

    glBindVertexArray(*vao);

    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), NULL);
    glEnableVertexAttribArray(0);

    // Texture coordinates (u, v)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void *)(3 * sizeof(f32)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

}