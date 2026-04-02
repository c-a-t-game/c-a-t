#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

void keybind_add_entry(const char* name);
void keybind_remove_entry(const char* name);
void keybind_add(const char* name, int keybind);
void keybind_remove(const char* name, int keybind);
int* keybind_get(const char* name, int* count);
const char** keybind_get_entries(int* count);

bool keybind_down(const char* name);
bool keybind_pressed(const char* name);
bool keybind_released(const char* name);
bool keybind_mouse_down(int button);
bool keybind_mouse_pressed(int button);
bool keybind_mouse_released(int button);
float keybind_mouse_x();
float keybind_mouse_y();

void keybind_set_mouse_scale(float scale);
float keybind_get_mouse_scale();

bool keybind_check(int bind);
int keybind_mouse();

void keybind_update();

#endif
