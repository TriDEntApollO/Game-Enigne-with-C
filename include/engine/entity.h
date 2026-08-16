#ifndef GAME_ENGINE_ENTITY_H
#define GAME_ENGINE_ENTITY_H

#include <stdbool.h>
#include <linmath.h>

#include "types.h"
#include "physics.h"


typedef struct entity {
    usize body_id;
    bool is_active;
} Entity;

void entity_init(void);
usize entity_create(vec2 position, vec2 size, vec2 velocity, u8 collision_layer, u8 collision_mask, OnHit on_hit, OnHitStatic on_hit_static);
Entity *entity_get_active(usize id);
usize entity_count(void);


#endif //GAME_ENGINE_ENTITY_H