#ifndef GAME_ENGINE_CONFIG_H
#define GAME_ENGINE_CONFIG_H

#include "input.h"
#include "types.h"

typedef struct config_state {
    u8 keybinds[6];
} ConfigState;

void config_init(void);
void config_key_bind(InputKey key, const char *key_name);


#endif // GAME_ENGINE_CONFIG_H