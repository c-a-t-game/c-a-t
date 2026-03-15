#ifndef SCRIPTS_H
#define SCRIPTS_H

#include <stdint.h>
#include <stdbool.h>

void scripts_init();
bool scripts_compile();
uint64_t scripts_compile_time();
void* scripts_get(const char* name);
void scripts_header(const char* filename, const char* data);
void scripts_add(const char* code, const char* filename);

#endif