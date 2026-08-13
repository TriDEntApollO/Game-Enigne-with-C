#include <SDL3/SDL.h>
#include <engine/io.h>
#include <engine/util.h>
#include <engine/global.h>
#include <engine/config.h>


static const char *CONFIG_DEFAULT = 
    "[controls]\n"
    "left = A\n"
    "right = D\n"
    "up = W\n"
    "down = S\n"
    "space = Space\n"
    "escape = Escape\n"
    "\n";


static char tmp_buffer[256] = {0};

static char *config_get_value(const char* config_buffer, const char *value) {
    char *line = strstr(config_buffer, value);
    if (!line) {
        ERROR_EXIT("could not find value '%s' in config file. Try deleting config.ini and restarting the game.\n", value);
    }

    size_t len = strlen(line);
    char *end = line + len;

    char *curr = line;
    char *tmp_ptr = tmp_buffer;

    while (*curr != '=' && curr < end) {
        ++curr;
    }
    ++curr;
    while (*curr == ' ' && curr < end) {
        ++curr;
    }

    while (*curr != '\n' && curr < end) {
        *tmp_ptr++ = *curr++;
    }
    *(tmp_ptr+1) = '\0';

    return tmp_buffer;
}


static load_controls(const char *config_buffer, size_t size) {
    config_key_bind(INPUT_KEY_LEFT, config_get_value(config_buffer, "left"));
    config_key_bind(INPUT_KEY_RIGHT, config_get_value(config_buffer, "right"));
    config_key_bind(INPUT_KEY_UP, config_get_value(config_buffer, "up"));
    config_key_bind(INPUT_KEY_DOWN, config_get_value(config_buffer, "down"));
    config_key_bind(INPUT_KEY_SPACE, config_get_value(config_buffer, "space"));
    config_key_bind(INPUT_KEY_ESCAPE, config_get_value(config_buffer, "escape"));
}

static int config_load(void) {
    File file_config = io_file_read("./config.ini");
    if (!file_config.is_valid) {
        return 1;
    }

    load_controls(file_config.data, file_config.size);
    io_file_free(&file_config);

    return 0;
}


void config_init(void) {
    if (config_load() == 0) {
        return; 
    }

    io_file_write((void *)CONFIG_DEFAULT, strlen(CONFIG_DEFAULT), "./config.ini");

    if (config_load() != 0) {
        ERROR_EXIT("could not create or load config file.\n");
    }

}

void config_key_bind(InputKey key, const char *key_name) {
    SDL_Scancode scan_code = SDL_GetScancodeFromName(key_name);
    if (scan_code == SDL_SCANCODE_UNKNOWN) {
        ERROR_EXIT("invalid key name '%s' in config file. Try deleting config.ini and restarting the game.\n", key_name);
    }

    global.config.keybinds[key] = scan_code;

}