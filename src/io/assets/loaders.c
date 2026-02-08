#include <stdint.h>
#include <string.h>
#include <dlfcn.h>

#include "main.h"

static const char* get_basename(const char* filename) {
    for (int i = strlen(filename) - 1; i >= 0; i--) {
        if (filename[i] == '/') return filename + i + 1;
    }
    return filename;
}

void* loader_txt(const char* filename, uint8_t* bytes, int size) {
    return strndup((char*)bytes, size);
}

void* loader_h(const char* filename, uint8_t* bytes, int size) {
    char* data = strndup((char*)bytes, size);
    jitc_create_header(jitc_context, get_basename(filename), data);
    return data;
}

void* loader_c(const char* filename, uint8_t* bytes, int size) {
    add_compile_job(filename);
    return strndup((char*)bytes, size);
}
