#include <stdio.h>
#include <glad/gl.h>
#include <engine/io.h>
#include <engine/util.h>

#include "render_internal.h"

#define LOG_SIZE 512

u32 render_shader_create(const char *path_vertex, const char *path_fragment) {
    int success;
    char log[LOG_SIZE];
    
    File file_vertex = io_file_read(path_vertex);
    if (!file_vertex.is_valid) {
        ERROR_EXIT("failed to read vertex shader file: %s", path_vertex);
    }

    u32 shader_vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader_vertex, 1, (const char *const *)&file_vertex.data, NULL);
    glCompileShader(shader_vertex);
    glGetShaderiv(shader_vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader_vertex, LOG_SIZE, NULL, log);
        ERROR_EXIT("failed to compile vertex shader: %s", log);
    }

    File file_fragment = io_file_read(path_fragment);
    if (!file_fragment.is_valid) {
        ERROR_EXIT("failed to read fragment shader file: %s", path_vertex);
    }

    u32 shader_fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader_fragment, 1, (const char *const *)&file_fragment.data, NULL);
    glCompileShader(shader_fragment);
    glGetShaderiv(shader_fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader_fragment, LOG_SIZE, NULL, log);
        ERROR_EXIT("failed to compile fragment shader: %s", log);
    }

    u32 shader = glCreateProgram();
    glAttachShader(shader, shader_vertex);
    glAttachShader(shader, shader_fragment);
    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader, LOG_SIZE, NULL, log);
        ERROR_EXIT("failed to link shader program: %s", log);
    }

    io_file_free(&file_vertex);
    io_file_free(&file_fragment);

    return shader;
}