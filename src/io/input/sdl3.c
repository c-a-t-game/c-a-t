#include <SDL3/SDL.h>
#include <SDL3/SDL_scancode.h>

#include "io/input.h"
#include "io/graphics.h"

float keybind_mouse_x() {
    float x;
    SDL_GetMouseState(&x, NULL);
    return x / keybind_get_mouse_scale() / graphics_get_dpi_scale(NULL);
}

float keybind_mouse_y() {
    float y;
    SDL_GetMouseState(NULL, &y);
    return y / keybind_get_mouse_scale() / graphics_get_dpi_scale(NULL);
}

int keybind_mouse() {
    return SDL_GetMouseState(NULL, NULL);
}

bool keybind_check(int bind) {
    return SDL_GetKeyboardState(NULL)[bind];
}
