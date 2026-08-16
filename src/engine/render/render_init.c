#include <glad/gl.h>
#include <SDL3/SDL.h>
#include <engine/util.h>
#include <engine/global.h>

#include "render_internal.h"


const char *DEFAULT_VERTEX_PATH = "./src/engine/shaders/default.vert";
const char *DEFAULT_FRAGMENT_PATH = "./src/engine/shaders/default.frag";
const char *BATCH_QUAD_VERTEX_PATH = "./src/engine/shaders/batch_quad.vert";
const char *BATCH_QUAD_FRAGMENT_PATH = "./src/engine/shaders/batch_quad.frag";

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

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        const char *error = SDL_GetError();
        SDL_DestroyWindow(window);
        SDL_Quit();
        ERROR_EXIT("error: failed to create OpenGL context: %s\n", error);
    }
    // global.render.gl_context = gl_context;

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

void render_init_shaders(u32 *shader_default, u32 *shader_batch, f32 render_width, f32 render_height) {
    mat4x4 projection;
    *shader_default = render_shader_create(DEFAULT_VERTEX_PATH, DEFAULT_FRAGMENT_PATH);
    *shader_batch = render_shader_create(BATCH_QUAD_VERTEX_PATH, BATCH_QUAD_FRAGMENT_PATH);

    mat4x4_ortho(projection, 0, render_width, 0, render_height, -2, 2);

    glUseProgram(*shader_default);
    glUniformMatrix4fv(
        glGetUniformLocation(*shader_default, "projection"), 
        1, 
        GL_FALSE, 
        (const GLfloat *)projection
    );

    glUseProgram(*shader_batch);
    glUniformMatrix4fv(
        glGetUniformLocation(*shader_batch, "projection"), 
        1, 
        GL_FALSE, 
        (const GLfloat *)projection
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

void render_init_batch_quads(u32 *vao, u32 *vbo, u32 *ebo) {
    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);    

    u32 indices[MAX_BATCH_ELEMENTS];
    for (u32 i = 0, offset = 0; i < MAX_BATCH_QUADS; i += 6, offset += 4) {
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;
        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;
    }

    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_BATCH_VERTICES * sizeof(BatchVertex), NULL, GL_DYNAMIC_DRAW);

    // Position (x, y)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(BatchVertex), (void *)offsetof(BatchVertex, position));
    glEnableVertexAttribArray(0);

    // Texture coordinates (u, v)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(BatchVertex), (void *)offsetof(BatchVertex, uvs));
    glEnableVertexAttribArray(1);

    // Color (r, g, b, a)
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(BatchVertex), (void *)offsetof(BatchVertex, color));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_BATCH_ELEMENTS * sizeof(u32), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void render_init_line(u32 *vao, u32 *vbo) {
    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(f32), NULL, GL_STATIC_DRAW);

    // Position (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), NULL);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}