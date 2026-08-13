#include <glad/gl.h>
#include <engine/render.h>
#include <engine/global.h>

#include "render_internal.h"


static RenderStateInternal state = {0};

void render_init(void) {
    global.render.width = 800;
    global.render.height = 600;
    global.render.window = render_init_window(global.render.width, global.render.height);
    
    render_init_quad(&state.vao_quad, &state.vbo_quad, &state.ebo_quad);
    render_init_shaders(&state);
    render_init_color_texture(&state.texture_color);
}

void render_begin(void) {
    const GLfloat color[] = { 0.08f, 0.1f, 0.1f, 1.0f };
    glClearBufferfv(GL_COLOR, 0, color);
}

void render_end(void) {
    SDL_GL_SwapWindow(global.render.window);
}

void render_quad(vec2 pos, vec2 size, vec4 color) {
    glUseProgram(state.shader_default);
    
    mat4x4 model;
    mat4x4_identity(model);
 
    mat4x4_translate(model, pos[0], pos[1], 0);
    mat4x4_scale_aniso(model, model, size[0], size[1], 1);

    glUniformMatrix4fv(
        glGetUniformLocation(state.shader_default, "model"), 
        1, 
        GL_FALSE, 
        &model[0][0]
    );

    glUniform4fv(
        glGetUniformLocation(state.shader_default, "color"), 
        1, 
        color
    );


    glBindVertexArray(state.vao_quad);


    glBindTexture(GL_TEXTURE_2D, state.texture_color);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
}