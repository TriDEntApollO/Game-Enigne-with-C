#ifndef GAME_ENGINE_PHYSICS_INTERNAL_H
#define GAME_ENGINE_PHYSICS_INTERNAL_H

#include <arraylist.h>
#include <engine/types.h>

typedef struct physics_state_internal {
    f32 gravity;
    f32 terminal_velocity;
    ArrayList *bodies;
    ArrayList *static_bodies;
} PhysicsStateInternal;

#endif //GAME_ENGINE_PHYSICS_INTERNAL_H