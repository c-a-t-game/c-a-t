#include <SDL3/SDL.h>

#include "io/input.h"

bool keybind_check(int bind) {
    return SDL_GetKeyboardState(NULL)[bind];
}
