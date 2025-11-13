#include "io/input.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    const char* name;
    int* keybinds;
    int size, capacity;
    bool down, prev_down;
} KeybindEntry;
static KeybindEntry* entries = NULL;
static int entries_size = 0, entries_capacity = 4;

static int compare_str(const void* a, const void* b) {
    return strcmp(*(char**)a, *(char**)b);
}

static int compare_int(const void* a, const void* b) {
    return *(int*)b - *(int*)a;
}

void keybind_add_entry(const char* name) {
    if (!entries) entries = malloc(sizeof(KeybindEntry) * entries_capacity);
    if (bsearch(&name, entries, entries_size, sizeof(KeybindEntry), compare_str)) return;
    KeybindEntry entry = (KeybindEntry){ name, malloc(sizeof(int) * 4), 0, 4 };
    if (entries_size > 0 && !entries[0].name) entries[0] = entry;
    else {
        if (entries_size == entries_capacity) {
            entries_capacity *= 2;
            entries = realloc(entries, sizeof(KeybindEntry) * entries_capacity);
        }
        entries[entries_size++] = entry;
    }
    qsort(entries, entries_size, sizeof(KeybindEntry), compare_str);
}

void keybind_remove_entry(const char* name) {
    KeybindEntry* entry = bsearch(&name, entries, entries_size, sizeof(KeybindEntry), compare_str);
    if (!entry) return;
    free(entry->keybinds);
    entry->name = NULL;
    qsort(entries, entries_size, sizeof(KeybindEntry), compare_str);
}

void keybind_add(const char* name, int keybind) {
    if (!name) return;
    KeybindEntry* entry = bsearch(&name, entries, entries_size, sizeof(KeybindEntry), compare_str);
    if (!entry) {
        keybind_add_entry(name);
        entry = bsearch(&name, entries, entries_size, sizeof(KeybindEntry), compare_str);
    }
    if (entry->size > 0 && entry->keybinds[0] == 0) entry->keybinds[0] = keybind;
    else {
        if (entry->size == entry->capacity) {
            entry->capacity *= 2;
            entry->keybinds = realloc(entry->keybinds, sizeof(int) * entry->capacity);
        }
        entry->keybinds[entry->size++] = keybind;
    }
    qsort(entry->keybinds, entry->size, sizeof(int), compare_int);
}

void keybind_remove(const char* name, int keybind) {
    if (!name) return;
    KeybindEntry* entry = bsearch(&name, entries, entries_size, sizeof(KeybindEntry), compare_str);
    if (!entry) return;
    int* ptr = bsearch(&keybind, entry->keybinds, entry->size, sizeof(int), compare_int);
    if (!ptr) return;
    *ptr = 0;
    qsort(entry->keybinds, entry->size, sizeof(int), compare_int);
}

int* keybind_get(const char* name, int* count) {
    if (!name) return NULL;
    KeybindEntry* entry = bsearch(&name, entries, entries_size, sizeof(KeybindEntry), compare_str);
    if (!entry) return NULL;
    int *ptr = NULL, *head = NULL;
    for (int i = 0; i < entry->size; i++) {
        if (entry->keybinds[i] != 0 && !head) {
            *count = entry->size - i;
            ptr = head = malloc(sizeof(int) * *count);
        }
        if (head) *head++ = entry->keybinds[i];
    }
    return ptr;
}

const char** keybind_get_entries(int* count) {
    const char **ptr = NULL, **head = NULL;
    for (int i = 0; i < entries_size; i++) {
        if (entries[i].name && !head) {
            *count = entries_size - i;
            ptr = head = malloc(sizeof(char*) * *count);
        }
        if (head) *head++ = entries[i].name;
    }
    return ptr;
}

void keybind_update() {
    for (int i = 0; i < entries_size; i++) {
        KeybindEntry* entry = &entries[i];
        if (!entry->name) continue;
        bool down = false;
        for (int j = 0; j < entry->size && !down; j++) {
            if (entry->keybinds[j] == 0) continue;
            if (keybind_check(entry->keybinds[j])) down = true;
        }
        entry->prev_down = entry->down;
        entry->down = down;
    }
}

bool keybind_down(const char* name) {
    if (!name) return false;
    KeybindEntry* entry = bsearch(&name, entries, entries_size, sizeof(KeybindEntry), compare_str);
    if (!entry) return false;
    return entry->down;
}

bool keybind_pressed(const char* name) {
    if (!name) return false;
    KeybindEntry* entry = bsearch(&name, entries, entries_size, sizeof(KeybindEntry), compare_str);
    if (!entry) return false;
    return entry->down && !entry->prev_down;
}

bool keybind_released(const char* name) {
    if (!name) return false;
    KeybindEntry* entry = bsearch(&name, entries, entries_size, sizeof(KeybindEntry), compare_str);
    if (!entry) return false;
    return !entry->down && entry->prev_down;
}

bool keybind_check(int bind) { return false; }
