#ifndef GAME_ENGINE_UTIL_H
#define GAME_ENGINE_UTIL_H

#include <stdio.h>

#define ERROR_EXIT(...) \
    do { \
        fprintf(stderr, "error: " __VA_ARGS__), fputc('\n', stderr); \
        exit(1); \
    } while (0)

#define ERROR_RETURN(R, ...) \
    do { \
        fprintf(stderr, "error: " __VA_ARGS__), fputc('\n', stderr); \
        return R; \
    } while (0)


#endif // GAME_ENGINE_UTIL_H