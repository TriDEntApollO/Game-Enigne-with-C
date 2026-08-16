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


#define RED (vec4){1, 0, 0, 1}
#define BLUE (vec4){0, 0, 1, 1}
#define CYAN (vec4){0, 1, 1, 1}
#define WHITE (vec4){1, 1, 1, 1}
#define BLACK (vec4){0, 0, 0, 1}
#define GREEN (vec4){0, 1, 0, 1}
#define YELLOW (vec4){1, 1, 0, 1}
#define LIME (vec4){0.5f, 1, 0, 1}
#define MAGENTA (vec4){1, 0, 1, 1}
#define ORANGE (vec4){1, 0.5f, 0, 1}
#define TEAL (vec4){0, 0.5f, 0.5f, 1}
#define PINK (vec4){1, 0.75f, 0.8f, 1}
#define PURPLE (vec4){0.5f, 0, 0.5f, 1}
#define GRAY (vec4){0.5f, 0.5f, 0.5f, 1}
#define BROWN (vec4){0.6f, 0.4f, 0.2f, 1}
#define TURQUOISE (vec4){0, 0.5f, 0.5f, 1}

#endif // GAME_ENGINE_UTIL_H