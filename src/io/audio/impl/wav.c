#include "io/audio.h"

#include <stdlib.h>
#include <string.h>

static void* wav_init(void* ctx) {
    int* ptr = malloc(sizeof(int));
    *ptr = 0;
    return ptr;
}

static void wav_free(void* ctx, void* inst) {
    free(inst);
}

static bool wav_play(void* ctx, void* inst, AudioSample* samples, int num_samples) {
    int* ptr = (int*)inst;
    int remaining = *(int*)ctx / sizeof(AudioSample) - *ptr;
    int to_write = num_samples < remaining ? num_samples : remaining;
    if (to_write == 0) return false;
    memcpy(samples, (char*)ctx + 4 + *ptr * sizeof(AudioSample), to_write * sizeof(AudioSample));
    memset(samples + to_write, 0, (num_samples - to_write) * sizeof(AudioSample));
    *ptr += to_write;
    return true;
}

static void wav_rate(void* ctx, void* inst, float rate) {}

static void wav_seek(void* ctx, void* inst, float sec) {
    *(int*)inst = sec * SAMPLE_RATE;
}

void* loader_wav(uint8_t* bytes, int size) {
    AudioSource* source = malloc(sizeof(AudioSource));
    source->context = bytes + 40; // skip over header but keep number of bytes in data block
    source->init = wav_init;
    source->free = wav_free;
    source->play = wav_play;
    source->rate = wav_rate;
    source->seek = wav_seek;
    return source;
}