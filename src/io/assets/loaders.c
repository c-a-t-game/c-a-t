#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

void* loader_txt(uint8_t* bytes, int size) {
    return strndup((char*)bytes, size);
}

void* loader_paw(uint8_t* bytes, int size) {
    char* data = strndup((char*)bytes, size);
    void* symbol = dlsym(RTLD_DEFAULT, data);
    free(data);
    return symbol;
}
