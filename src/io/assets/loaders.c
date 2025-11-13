#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "pawscript.h"

void* loader_txt(uint8_t* bytes, int size) {
    return strndup((char*)bytes, size);
}

void* loader_paw(uint8_t* bytes, int size) {
    char* data = strndup((char*)bytes, size);
    PawScriptContext* context = pawscript_create_context();
    PawScriptError* error;
    if ((error = pawscript_run(context, data))) pawscript_log_error(error, stderr);
    free(data);
    return context;
}
