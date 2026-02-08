#ifndef MAIN_H
#define MAIN_H

#include "jitc/jitc.h"

extern jitc_context_t* jitc_context;

void add_compile_job(const char* filename);

#endif
