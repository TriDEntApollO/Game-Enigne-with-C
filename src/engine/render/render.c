#include <glad/gl.h>
#include <arraylist.h>
#include <engine/util.h>
#include <engine/render.h>
#include <engine/global.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "render_internal.h"

static f32 window_width = 1920;
static f32 window_height = 1080;
static f32 render_width = 640;
static f32 render_height = 360;
static f32 render_scale = 3;

static u32 vao_quad;
static u32 vbo_quad;
static u32 ebo_quad;
static u32 vao_line;
static u32 vbo_line;
static u32 shader_default;
static u32 texture_color;

static u32 vao_batch;
static u32 vbo_batch;
static u32 ebo_batch;
static u32 shader_batch;
static ArrayList *batch_list;

static void append_quad(vec2 position, vec2 size, vec4 texture_coordinates, vec4 color) {
    vec4 uvs = {0.0f, 0.0f, 1.0f, 1.0f};

    if (texture_coordinates != NULL) {
        memcpy(uvs, texture_coordinates, sizeof(vec4));
    }

    arraylist_append(batch_list, &(BatchVertex){ 
        .position = { position[0], position[1] }, 
        .uvs = { uvs[0], uvs[1] }, 
        .color = { color[0], color[1], color[2], color[3] } 
    });

    arraylist_append(batch_list, &(BatchVertex){ 
        .position = { position[0] + size[0], position[1] }, 
        .uvs = { uvs[2], uvs[1] }, 
        .color = { color[0], color[1], color[2], color[3] } 
    });

    arraylist_append(batch_list, &(BatchVertex){ 
        .position = { position[0] + size[0], position[1] + size[1] }, 
        .uvs = { uvs[2], uvs[3] }, 
        .color = { color[0], color[1], color[2], color[3] } 
    });

    arraylist_append(batch_list, &(BatchVertex){ 
        .position = { position[0], position[1] + size[1] }, 
        .uvs = { uvs[0], uvs[3] }, 
        .color = { color[0], color[1], color[2], color[3] } 
    });
}

static void render_batch(BatchVertex *vertices, usize count, u32 texture_id) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_batch);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(BatchVertex) * count, vertices);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glUseProgram(shader_batch);
    glBindVertexArray(vao_batch);

    glDrawElements(GL_TRIANGLES, (count >> 2) * 6, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
}

static void calculate_sprite_texture_coordinates(vec4 texture_coordinates, SpriteSheet *sprite_sheet, f32 row, f32 column) {
    f32 width = 1.0f / (sprite_sheet->width / sprite_sheet->cell_width);
    f32 height = 1.0f / (sprite_sheet->height / sprite_sheet->cell_height);
    f32 x = column * width;
    f32 y = row * height;

    texture_coordinates[0] = x;
    texture_coordinates[1] = y;
    texture_coordinates[2] = x + width;
    texture_coordinates[3] = y + height;
}


SDL_Window *render_init(void) {
    SDL_Window *window = render_init_window(window_width, window_height);
    
    render_init_quad(&vao_quad, &vbo_quad, &ebo_quad);
    render_init_batch_quads(&vao_batch, &vbo_batch, &ebo_batch);
    render_init_line(&vao_line, &vbo_line);
    render_init_shaders(&shader_default, &shader_batch, render_width, render_height);
    render_init_color_texture(&texture_color);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    batch_list = arraylist_create(sizeof(BatchVertex), 8);

    stbi_set_flip_vertically_on_load(1);
    
    return window;
}

void render_begin(void) {
    const GLfloat color[] = { 0.08f, 0.1f, 0.1f, 1.0f };
    glClearBufferfv(GL_COLOR, 0, color);

    batch_list->len = 0;
}

void render_end(SDL_Window *window, u32 batch_texture_id) {
    render_batch(batch_list->items, batch_list->len, batch_texture_id);
    SDL_GL_SwapWindow(window);
}

void render_line_segment(vec2 start, vec2 end, vec4 color) {
    glUseProgram(shader_default);
    glLineWidth(3.0f);

    f32 x = end[0] - start[0];
    f32 y = end[1] - start[1];
    f32 line[6] = {0, 0, 0, x, y, 0};

    mat4x4 model;
    mat4x4_translate(model, start[0], start[1], 0);

    glUniformMatrix4fv(
        glGetUniformLocation(shader_default, "model"), 
        1, 
        GL_FALSE, 
        &model[0][0]
    );
    glUniform4fv(
        glGetUniformLocation(shader_default, "color"), 
        1, 
        color
    );

    glBindTexture(GL_TEXTURE_2D, texture_color);
    glBindVertexArray(vao_line);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_line);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(line), line);
    glDrawArrays(GL_LINES, 0, 2);

    glBindVertexArray(0);
}

void render_quad_line(vec2 pos, vec2 size, vec4 color) {
    vec2 half_size = {size[0] * 0.5f, size[1] * 0.5f};
    vec2 points[4] = {
        {pos[0] - half_size[0], pos[1] - half_size[1]},
        {pos[0] + half_size[0], pos[1] - half_size[1]},
        {pos[0] + half_size[0], pos[1] + half_size[1]},
        {pos[0] - half_size[0], pos[1] + half_size[1]}
    };

    render_line_segment(points[0], points[1], color);
    render_line_segment(points[1], points[2], color);
    render_line_segment(points[2], points[3], color);
    render_line_segment(points[3], points[0], color);
}

void render_quad(vec2 pos, vec2 size, vec4 color) {
    glUseProgram(shader_default);
    
    mat4x4 model;
    // mat4x4_identity(model);
 
    mat4x4_translate(model, pos[0], pos[1], 0);
    mat4x4_scale_aniso(model, model, size[0], size[1], 1);

    glUniformMatrix4fv(
        glGetUniformLocation(shader_default, "model"), 
        1, 
        GL_FALSE, 
        &model[0][0]
    );

    glUniform4fv(
        glGetUniformLocation(shader_default, "color"), 
        1, 
        color
    );

    glBindVertexArray(vao_quad);
    glBindTexture(GL_TEXTURE_2D, texture_color);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
}

void render_aabb(f32 *aabb, vec4 color) {
    vec2 size;
    vec2_scale(size, &aabb[2], 2.0f);
    render_quad_line(&aabb[0], size, color);
}

f32 render_get_scale(void) {
    return render_scale;
}

void render_sprite_sheet_init(SpriteSheet *sprite_sheet, const char *path, f32 cell_width, f32 cell_height) {
    glGenTextures(1, &sprite_sheet->texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite_sheet->texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, channel_count;
    u8 *image_data = stbi_load(path, &width, &height, &channel_count, 0);
    if (!image_data) {
        ERROR_EXIT("failed to load image: %s\n", path);
        return;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    sprite_sheet->width = (f32)width;
    sprite_sheet->height = (f32)height;
    sprite_sheet->cell_width = cell_width;
    sprite_sheet->cell_height = cell_height;
}

void render_sprite_sheet_frame(SpriteSheet *sprite_sheet, f32 row, f32 column, vec2 position) {
    vec4 uvs;
    calculate_sprite_texture_coordinates(uvs, sprite_sheet, row, column);

    vec2 size = { sprite_sheet->cell_width, sprite_sheet->cell_height };
    vec2 bottom_left = { position[0] - size[0] * 0.5f, position[1] - size[1] * 0.5f };

    append_quad(bottom_left, size, uvs, WHITE);
}