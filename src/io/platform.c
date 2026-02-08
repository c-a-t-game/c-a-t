#include "platform.h"

#include <stdbool.h>
#include <stddef.h>

uint64_t get_millis() { return get_micros() / 1000; }
uint64_t get_micros() { return 0; }
