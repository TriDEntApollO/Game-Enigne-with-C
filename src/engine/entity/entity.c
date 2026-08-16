#include <engine/entity.h>
#include <engine/arrayList.h>


static ArrayList *entities;

void entity_init(void) {
    entities = arraylist_create(sizeof(Entity), 0);
}

usize entity_create(vec2 position, vec2 size, vec2 velocity, u8 collision_layer, u8 collision_mask, OnHit on_hit, OnHitStatic  on_hit_static) {
    Entity entity = {
        .body_id = physics_body_create(position, size, velocity, collision_layer, collision_mask, on_hit, on_hit_static),
        .is_active = true
    };

    return arraylist_append(entities, &entity);
}

Entity *entity_get_active(usize id) {
    return (Entity *)arraylist_get(entities, id);
}

usize entity_count(void) {
    return entities->len;
}