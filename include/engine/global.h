#ifndef GAME_ENGINE_GLOBAL_H
#define GAME_ENGINE_GLOBAL_H

#include "time.h"
#include "input.h"
#include "render.h"
#include "config.h"


typedef struct global_state {
    TimeState time;
    InputState input;
    ConfigState config;
} Global;

extern Global global;

#endif //GAME_ENGINE_GLOBAL_H