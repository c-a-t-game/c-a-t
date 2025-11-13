#ifndef LOADERS_H
#define LOADERS_H

#include <stdint.h>

#define LOADER(x) void* loader_##x(uint8_t* data, int len);
#include "loader_def.h"
#undef LOADER

#endif
