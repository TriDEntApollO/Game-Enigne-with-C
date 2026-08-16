#ifndef GAME_ENGINE_ARRAYLIST_H
#define GAME_ENGINE_ARRAYLIST_H

#include "engine/types.h"


typedef struct {
    usize len;
    usize capacity;
    usize item_size;
    void *items;
} ArrayList;

ArrayList *arraylist_create(usize item_size, usize initial_capacity);
usize arraylist_append(ArrayList *list, const void *item);
void *arraylist_get(ArrayList *list, usize index);
u8 arraylist_remove(ArrayList *list, usize index);

#endif //GAME_ENGINE_ARRAYLIST_H