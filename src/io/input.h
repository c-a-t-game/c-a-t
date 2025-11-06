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

bool keybind_check(int bind);

#endif
