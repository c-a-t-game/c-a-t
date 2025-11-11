#include "io/audio.h"

#include <string.h>
#include <stdlib.h>

struct AudioInstance {
    void* data;
    bool playing;
    bool oneshot;
    bool do_free;
    AudioSource* source;
};

static AudioInstance* instances;
static int instances_size = 0, instances_capacity = 4;

static int compare_ptr(const void* a, const void* b) {
    return *(uintptr_t*)b - *(uintptr_t*)a;
}

void audio_update(AudioSample* out, int samples) {
    memset(out, 0, sizeof(AudioSample) * samples);
    for (int i = 0; i < instances_size; i++) {
        if (!instances[i].data) continue;
        if (!instances[i].playing) continue;
        AudioSample buf[samples];
        if (instances[i].do_free || (
            !instances[i].source->play(instances[i].source->context, instances[i].data, buf, samples)
            && instances[i].oneshot
        )) {
            instances[i].source->free(instances[i].source->context, instances[i].data);
            instances[i].data = NULL;
            continue;
        }
        for (int j = 0; j < samples; j++) {
            int sample = out[j] + buf[j];
            if (sample < -32768) sample = -32768;
            if (sample > 32767) sample = 32767;
            out[j] = sample;
        }
    }
}

void audio_stop(AudioInstance* instance) {
    instance->do_free = true;
}

void audio_pause(AudioInstance* instance) {
    instance->playing = false;
}

void audio_resume(AudioInstance* instance) {
    instance->playing = true;
}

void audio_seek(AudioInstance* instance, float sec) {
    instance->source->seek(instance->source->context, instance->data, sec);
}

void audio_rate(AudioInstance* instance, float factor) {
    instance->source->rate(instance->source->context, instance->data, factor);
}

void audio_play_oneshot(AudioSource* source) {
    AudioInstance* instance = audio_play(source);
    instance->oneshot = true;
}

AudioInstance* audio_play(AudioSource* source) {
    if (!instances) instances = malloc(sizeof(AudioInstance) * instances_capacity);
    AudioInstance* instance = NULL;
    for (int i = 0; i < instances_size && !instance; i++) {
        if (!instances[i].data) instance = &instances[i];
    }
    if (!instance) {
        if (instances_size == instances_capacity) {
            instances_capacity *= 2;
            instances = realloc(instances, sizeof(AudioInstance) * instances_capacity);
        }
        instance = &instances[instances_size++];
    }
    instance->data = source->init(source->context);
    instance->source = source;
    instance->playing = true;
    instance->oneshot = false;
    instance->do_free = false;
    return instance;
}

void audio_init() {}
