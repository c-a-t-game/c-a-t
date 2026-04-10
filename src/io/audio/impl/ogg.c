#include "io/audio.h"
#include "stb_vorbis.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    int size;
    uint8_t data[];
} File;

static void* ogg_init(void* ctx) {
    File* file = ctx;
    return stb_vorbis_open_memory(file->data, file->size, NULL, NULL);
}

static void ogg_free(void* ctx, void* inst) {
    stb_vorbis_close(inst);
}

static bool ogg_play(void* ctx, void* inst, AudioSample* samples, int num_samples) {
    if (stb_vorbis_get_samples_short(inst, 1, &samples, num_samples) == 0) return false;
    return true;
}

static void ogg_rate(void* ctx, void* inst, float rate) {}

static void ogg_seek(void* ctx, void* inst, float sec) {
    stb_vorbis_seek(inst, SAMPLE_RATE * sec);
}

void* loader_ogg(const char* filename, uint8_t* bytes, int size) {
    AudioSource* source = malloc(sizeof(AudioSource));
    File* file = malloc(sizeof(int) + size);
    file->size = size;
    memcpy(file + 1, bytes, size);
    source->context = file;
    source->init = ogg_init;
    source->free = ogg_free;
    source->play = ogg_play;
    source->rate = ogg_rate;
    source->seek = ogg_seek;
    return source;
}
