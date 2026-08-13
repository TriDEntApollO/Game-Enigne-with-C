#ifndef GAME_ENGINE_GLOBAL_H
#define GAME_ENGINE_GLOBAL_H

#include "time.h"
#include "input.h"
#include "render.h"
#include "config.h"


typedef struct {
    TimeState time;
    InputState input;
    ConfigState config;
    RenderState render;
} Global;

extern Global global;

#endif //GAME_ENGINE_GLOBAL_H