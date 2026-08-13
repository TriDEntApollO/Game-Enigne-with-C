#ifndef GAME_ENGINE_INPUT_H
#define GAME_ENGINE_INPUT_H

typedef enum {
    INPUT_KEY_LEFT,
    INPUT_KEY_RIGHT,
    INPUT_KEY_UP,
    INPUT_KEY_DOWN,
    INPUT_KEY_SPACE,
    INPUT_KEY_ESCAPE,
} InputKey;

typedef enum {
    KS_UNPRESSED,
    KS_PRESSED,
    KS_HELD,
} KeyState;

typedef struct {
    KeyState left;
    KeyState right;
    KeyState up;
    KeyState down;
    KeyState space;
    KeyState escape;
} InputState;

void input_update(void);


#endif // GAME_ENGINE_INPUT_h