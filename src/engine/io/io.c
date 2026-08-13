#include <stdio.h>
#include <errno.h>

#include <engine/io.h>
#include <engine/util.h>
#include <engine/types.h>

#define IO_READ_BUFFER_SIZE 2097152 // 20 MiB chunk buffer
#define IO_READ_ERROR_GENERAL "failed to read file '%s', errno: %d\n"
#define IO_READ_ERROR_MEMORY "not enough memory to read file '%s'\n"


File io_file_read(const char *path) {
    File file = {0};
    FILE *fp = fopen(path, "rb");

    if (!fp) {
        ERROR_RETURN(file, IO_READ_ERROR_GENERAL, path, errno);
    }

    char *data = NULL, *temp;
    size_t used = 0, size = 0, n;

    while (true) {
        if (used + IO_READ_BUFFER_SIZE + 1 > size) {
            size = used + IO_READ_BUFFER_SIZE + 1;
            
            if (size <= used) {
                free(data);
                fclose(fp);
                ERROR_RETURN(file, "input file '%s' too large\n", path);
            }

            temp = realloc(data, size);
            if (!temp) {
                free(data);
                fclose(fp);
                ERROR_RETURN(file, IO_READ_ERROR_MEMORY, path);
            }

            data = temp;
        }

        n = fread(data + used, 1, IO_READ_BUFFER_SIZE, fp);
        if (n == 0) {
            break;
        }

        used += n;
    }

    if (ferror(fp)) {
        free(data);
        fclose(fp);
        ERROR_RETURN(file, IO_READ_ERROR_GENERAL, path, errno);
    }

    temp = realloc(data, used + 1);
    if (!temp) {
        free(data);
        fclose(fp);
        ERROR_RETURN(file, IO_READ_ERROR_MEMORY, path);
    }

    fclose(fp);
    data = temp;
    data[used] = '\0';

    file.data = data;
    file.size = used;
    file.is_valid = true;

    return file;
}

void io_file_free(File *file) {
    if (file && file->is_valid) {
        free(file->data);
        file->data = NULL;
        file->size = 0;
        file->is_valid = false;
    }
}

int io_file_write(void *buffer, size_t size, const char *path) {
    FILE *fp = fopen(path, "wb");
    if (!fp || ferror(fp)) {
        ERROR_RETURN(1, "failed to open file '%s' for writing, errno: %d\n", path, errno);
    }

    size_t written = fwrite(buffer, 1, size, fp);
    fclose(fp);

    if (written != size) {
        ERROR_RETURN(1, "failed to write %zu bytes to file '%s', written %zu bytes, errno: %d\n", size, path, written, errno);
    }
    

    return 0;
}
