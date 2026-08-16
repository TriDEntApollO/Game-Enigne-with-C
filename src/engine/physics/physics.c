#include <linmath.h>
#include <engine/util.h>
#include <engine/global.h>
#include <engine/physics.h>

#include "physics_internal.h"


/* Internal Functions & Variables */
static f32 tick_rate;
static u32 iterations = 2;
static PhysicsStateInternal state;

void aabb_min_max(vec2 min, vec2 max, AABB aabb) {
    vec2_sub(min, aabb.position, aabb.half_size);
    vec2_add(max, aabb.position, aabb.half_size);
}

AABB aabb_minkowski_difference(AABB a, AABB b) {
    AABB result;
    vec2_sub(result.position, a.position, b.position);
    vec2_add(result.half_size, a.half_size, b.half_size);

    return result;
}

void aabb_penetration_vector(vec2 r, AABB aabb) {
    vec2 min, max;
    aabb_min_max(min, max, aabb);

    f32 min_dist = fabsf(min[0]);
    r[0] = min[0];
    r[1] = 0.0f;

    if (fabsf(max[0]) < min_dist) {
        min_dist = fabsf(max[0]);
        r[0] = max[0];
        r[1] = 0.0f;
    }

    if (fabsf(min[1]) < min_dist) {
        min_dist = fabsf(min[1]);
        r[0] = 0.0f;
        r[1] = min[1];
    }

    if (fabsf(max[1]) < min_dist) {
        r[0] = 0.0f;
        r[1] = max[1];
    }
}

Hit ray_intersect_aabb(vec2 position, vec2 magnitude, AABB aabb) {
    Hit hit = {0};
    vec2 min, max;
    aabb_min_max(min, max, aabb);

    f32 last_entry = -INFINITY;
    f32 first_exit = INFINITY;


    for (u8 i = 0; i < 2; i++) {
        if (magnitude[i] != 0) {
            f32 t1 = (min[i] - position[i]) / magnitude[i];
            f32 t2 = (max[i] - position[i]) / magnitude[i];

            last_entry = fmaxf(last_entry, fminf(t1, t2));
            first_exit = fminf(first_exit, fmaxf(t1, t2));
        } else if (position[i] <= min[i] || position[i] >= max[i]) {
            return hit;
        }
    }

    if (first_exit > last_entry && first_exit > 0 && last_entry < 1) {
        hit.position[0] = position[0] + magnitude[0] * last_entry;
        hit.position[1] = position[1] + magnitude[1] * last_entry;
        hit.is_hit = true;
        hit.time = last_entry;

        f32 dx = hit.position[0] - aabb.position[0];
        f32 dy = hit.position[1] - aabb.position[1];
        f32 px = aabb.half_size[0] - fabsf(dx);
        f32 py = aabb.half_size[1] - fabsf(dy);

        if (px < py) {
            hit.normal[0] = (dx > 0) - (dx < 0);
        } else {
            hit.normal[1] = (dy > 0) - (dy < 0);
        }   
    }

    return hit;
}

static void update_sweep_result(Hit *result, usize other_id, AABB a, AABB b, vec2 velocity, u8 a_collision_mask, u8 b_collision_layer) {
    if ((a_collision_mask & b_collision_layer) == 0) {
        return;
    }

    AABB sum_aabb = b;
        vec2_add(sum_aabb.half_size, sum_aabb.half_size, a.half_size);
        
        Hit hit = ray_intersect_aabb(a.position, velocity, sum_aabb);
        if (hit.is_hit) {   
            
            if (hit.time < result->time) {
                *result = hit;
            } else if (hit.time == result->time) {
                // Solve highest velocity axis first.
                if (fabsf(velocity[0]) > fabsf(velocity[1]) && hit.normal[0] != 0) {
                    *result = hit;
                } else if (fabsf(velocity[1]) > fabsf(velocity[0]) && hit.normal[1] != 0) {
                    *result = hit;
                }
            }

            result->other_id = other_id;
        }
}

static Hit sweep_static_bodies(Body *body, vec2 velocity) {
    Hit result = {.time = 0xBEEF};

    for (u32 i = 0; i < state.static_bodies->len; i++) {
        StaticBody *static_body = physics_static_body_get(i);
        update_sweep_result(&result, i, body->aabb, static_body->aabb, velocity, body->collision_mask, static_body->collision_layer);
    }

    return result;
}

static Hit sweep_bodies(Body *body, vec2 velocity) {
    Hit result = {.time = 0xBEEF};

    for (u32 i = 0; i < state.bodies->len; i++) {
        Body *other_body = physics_body_get(i);
        if (other_body != body) {
            update_sweep_result(&result, i, body->aabb, other_body->aabb, velocity, body->collision_mask, other_body->collision_layer);
        }
    }

    return result;
}

static void sweep_response(Body *body, vec2 velocity) {
    Hit hit = sweep_static_bodies(body, velocity);
    Hit hit_moving = sweep_bodies(body, velocity);

    if (hit_moving.is_hit) {
        if (body->on_hit != NULL) {
            body->on_hit(body, physics_body_get(hit_moving.other_id), hit_moving);
        }
    }

    if (hit.is_hit) {
        body->aabb.position[0] = hit.position[0];
        body->aabb.position[1] = hit.position[1];

        if (hit.normal[0] != 0) {
            body->aabb.position[1] += velocity[1];
            body->velocity[0] = 0;
        } else if (hit.normal[1] != 0) {
            body->aabb.position[0] += velocity[0];
            body->velocity[1] = 0;
        }

        if (body->on_hit_static != NULL) {
            body->on_hit_static(body, physics_static_body_get(hit.other_id), hit);
        }

    } else {
        vec2_add(body->aabb.position, body->aabb.position, velocity);
    }

}

static stationary_response(Body *body) {
    for (u32 i = 0; i < state.static_bodies->len; i++) {
        StaticBody *static_body = physics_static_body_get(i);
        AABB aabb = aabb_minkowski_difference(static_body->aabb, body->aabb);
        
        vec2 min, max;
        aabb_min_max(min, max, aabb);

        if (min[0] <= 0 && max[0] >= 0 && min[1] <= 0 && max[1] >= 0) {
            vec2 penetration_vector;
            aabb_penetration_vector(penetration_vector, aabb);
            vec2_add(body->aabb.position, body->aabb.position, penetration_vector);
        }
    }
}


/* Public API Functions */
void physics_init(void) {
    state.bodies = arraylist_create(sizeof(Body), 0);
    state.static_bodies = arraylist_create(sizeof(StaticBody), 0);

    state.gravity = -100.0f;
    state.terminal_velocity = -7000.0f;

    tick_rate = 1.0f / iterations;
}

void physics_update(void) {
    Body *body;

    for (u32 i = 0; i < state.bodies->len; i++) {
        body = (Body *)arraylist_get(state.bodies, i);

        body->velocity[1] += state.gravity;
        if (state.terminal_velocity > body->velocity[1]) {
            body->velocity[1] = state.terminal_velocity;
        }
        
        body->velocity[0] += body->acceleration[0];
        body->velocity[1] += body->acceleration[1];

        vec2 scaled_velocity;
        vec2_scale(scaled_velocity, body->velocity, tick_rate * global.time.delta);

        for (u32 j = 0; j < iterations; j++) {
            sweep_response(body, scaled_velocity);
            stationary_response(body);
        }
    }

}

usize physics_body_create(vec2 position, vec2 size, vec2 velocity, u8 collision_layer, u8 collision_mask, OnHit on_hit, OnHitStatic on_hit_static) {
    Body body = {
        .aabb = {
            .position = { position[0], position[1] },
            .half_size = { size[0] / 2.0f, size[1] / 2.0f }
        },
        .velocity = { velocity[0], velocity[1] },
        .acceleration = { 0.0f, 0.0f },
        .collision_layer = collision_layer,
        .collision_mask = collision_mask,
        .on_hit = on_hit,
        .on_hit_static = on_hit_static
    };

    
    if (arraylist_append(state.bodies, &body) == (usize)-1) {
        ERROR_RETURN(-1, "failed to add physics body\n");
    }

    return state.bodies->len - 1;
}

Body *physics_body_get(usize index) {
    return (Body *)arraylist_get(state.bodies, index);
}

usize physics_static_body_create(vec2 position, vec2 size, u8 collision_layer) {
    StaticBody static_body = {
        .aabb = {
            .position = { position[0], position[1] },
            .half_size = { size[0] / 2.0f, size[1] / 2.0f }
        },
        .collision_layer = collision_layer
    };
    
    if (arraylist_append(state.static_bodies, &static_body) == (usize)-1) {
        ERROR_RETURN(-1, "failed to add static physics body\n");
    }

    return state.static_bodies->len - 1;
}

StaticBody *physics_static_body_get(usize index) {
    return (StaticBody *)arraylist_get(state.static_bodies, index);
}

bool physics_point_intersect_aabb(vec2 point, AABB aabb) {
    vec2 min, max;
    aabb_min_max(min, max, aabb);

    return (point[0] >= min[0] && point[0] <= max[0] && 
           point[1] >= min[1] && point[1] <= max[1]);
}

bool physics_aabb_intersect_aabb(AABB a, AABB b) {
    vec2 min, max;
    aabb_min_max(min, max, aabb_minkowski_difference(a, b));

    return (min[0] <= 0 && max[0] >= 0 &&
            min[1] <= 0 && max[1] >= 0);
}