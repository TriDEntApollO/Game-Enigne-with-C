#ifndef GAME_ENGINE_TIME_H
#define GAME_ENGINE_TIME_H

#include "types.h"

typedef struct time_state {
    f32 delta;
    f32 now;
    f32 last;

    f32 frame_last;
    f32 frame_delay;
    f32 frame_time;

    u32 frame_rate;
    u32 frame_count;
} TimeState;

void time_update(void);
void time_update_late(void);
void time_init(u32 frame_rate);

#endif // GAME_ENGINE_TIME_H