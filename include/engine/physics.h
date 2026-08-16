#ifndef GAME_ENGINE_PHYSICS_H
#define GAME_ENGINE_PHYSICS_H

#include <stdbool.h>
#include <linmath.h>
#include "types.h"

typedef struct hit Hit;
typedef struct aabb AABB;
typedef struct body Body;
typedef struct static_body StaticBody;


typedef void (*OnHit)(Body *self, Body *other, Hit hit);
typedef void (*OnHitStatic)(Body *self, StaticBody *other, Hit hit);

struct aabb {
    vec2 position;
    vec2 half_size;
};

struct body {
    AABB aabb;
    vec2 velocity;
    vec2 acceleration;
    u8 collision_layer;
    u8 collision_mask;
    OnHit on_hit;
    OnHitStatic on_hit_static;
};

struct static_body {
    AABB aabb;
    u8 collision_layer;
};

struct hit {
    usize other_id;
    f32 time;
    vec2 position;
    vec2 normal;
    bool is_hit;
};



void physics_init(void);
void physics_update(void);

usize physics_body_create(vec2 position, vec2 size, vec2 velocity, u8 collision_layer, u8 collision_mask, OnHit on_hit, OnHitStatic on_hit_static);
Body *physics_body_get(usize index);
usize physics_static_body_create(vec2 position, vec2 size, u8 collision_layer);
StaticBody *physics_static_body_get(usize index);

bool physics_point_intersect_aabb(vec2 point, AABB aabb);
bool physics_aabb_intersect_aabb(AABB a, AABB b);


// Temporary exposure of internal functions for testing purposes. These should be removed in the future.
void aabb_min_max(vec2 min, vec2 max, AABB aabb);
AABB aabb_minkowski_difference(AABB a, AABB b);
void aabb_penetration_vector(vec2 r, AABB aabb);
Hit ray_intersect_aabb(vec2 position, vec2 magnitude, AABB aabb);

#endif //GAME_ENGINE_PHYSICS_H