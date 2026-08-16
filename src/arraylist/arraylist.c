#include <stdlib.h>
#include <string.h>
#include <arraylist.h>
#include <engine/util.h>


ArrayList *arraylist_create(usize item_size, usize initial_capacity) {
    ArrayList *list = (ArrayList *)malloc(sizeof(ArrayList));
    if (!list) {
        ERROR_RETURN(NULL, "failed to allocate memory for ArrayList\n");
    }
    list->len = 0;
    list->item_size = item_size;
    list->capacity = initial_capacity;
    list->items = malloc(item_size * initial_capacity);
    if (!list->items) {
        ERROR_EXIT("failed to allocate memory for ArrayList items\n");
    }

    return list;
}

usize arraylist_append(ArrayList *list, const void *item) {
    if (list->len >= list->capacity) {
        usize new_capacity = list->capacity == 0 ? 1 : list->capacity * 2;

        void *new_items = realloc(list->items, new_capacity * list->item_size);
        if (!new_items) {
            ERROR_RETURN(-1, "failed to reallocate memory for ArrayList items\n");
        }
        list->items = new_items;
        list->capacity = new_capacity;
    }

    usize index = list->len++;
    memcpy((u8 *)list->items + index * list->item_size, item, list->item_size);

    return index;
}

void *arraylist_get(ArrayList *list, usize index) {
    if (index >= list->len) {
        ERROR_RETURN(NULL, "index out of bounds: %zu (len: %zu)\n", index, list->len);
    }
    return (u8 *)list->items + index * list->item_size;
}

u8 arraylist_remove(ArrayList *list, usize index) {
    if (list->len == 0) {
        ERROR_RETURN(1, "list is empty\n");
    }
    if (index >= list->len) {
        ERROR_RETURN(1, "index out of bounds: %zu (len: %zu)\n", index, list->len);
    }

    if (list->len == 1) {
        list->len = 0;
        return 0;
    }

    --list->len;

    u8 *item_ptr = (u8 *)list->items + index * list->item_size;
    u8 *end_ptr = (u8 *)list->items + list->len * list->item_size;
    memcpy(item_ptr, end_ptr, list->item_size);
    
    return 0;
}