#ifndef GAME_ENGINE_IO_H
#define GAME_ENGINE_IO_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    char *data;
    size_t size;
    bool is_valid;
} File;

File io_file_read(const char *path);
void io_file_free(File *file);
int io_file_write(void *buffer, size_t size, const char *path);



#endif // GAME_ENGINE_IO_H