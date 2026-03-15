#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdbool.h>
#include <stdint.h>

typedef void(*FileWatchCallback)(const char* filename);

uint64_t get_millis();
uint64_t get_micros();

void watch_file(const char* filename, FileWatchCallback callback);
void check_watched_files();

#endif
