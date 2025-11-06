#include "io/audio.h"
#include "io/graphics.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void* loader_txt(uint8_t* data, int len) {
    return strndup((char*)data, len);
}

typedef struct {
    const char* name;
    int length;
    union {
        uint8_t* raw;
        void* data;
    };
} Asset;

typedef struct {
    const char* ext;
    void*(*loader)(uint8_t* data, int len);
} Loader;

static Asset assets[] = {
#include "data.h"
};

#define LOADER(ext) { #ext, loader_##ext }
static Loader loaders[] = {
    LOADER(txt),
    LOADER(png),
    LOADER(glsl),
};

static const char* get_extension(const char* name) {
    for (int i = strlen(name) - 1; i >= 0; i--) {
        if (name[i] == '/') return NULL;
        if (name[i] == '.') return name + i + 1;
    }
    return NULL;
}

static int compare_str(const void* a, const void* b) {
    return strcmp(*(char**)a, *(char**)b);
}

void load_assets() {
    int num_assets = sizeof(assets) / sizeof(Asset);
    int num_loaders = sizeof(loaders) / sizeof(Loader);
    qsort(assets, num_assets, sizeof(Asset), compare_str);
    for (int i = 0; i < sizeof(assets) / sizeof(Asset); i++) {
        Asset* asset = &assets[i];
        const char* ext = get_extension(asset->name);
        for (int j = 0; j < num_loaders; j++) {
            Loader* loader = &loaders[j];
            if (strcmp(ext, loader->ext) != 0) continue;
            asset->data = loader->loader(asset->raw, asset->length);
        }
    }
}

void* _get_asset(const char* name) {
    Asset* asset = bsearch(&name, assets, sizeof(assets) / sizeof(Asset), sizeof(Asset), compare_str);
    if (!asset) {
        printf("Asset '%s' not found\n", name);
        return NULL;
    }
    return asset->data;
}
