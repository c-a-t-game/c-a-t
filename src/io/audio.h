#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>
#include <stdbool.h>

#define SAMPLE_RATE 44100

typedef int16_t AudioSample;
typedef struct AudioInstance AudioInstance;
typedef struct {
    void* context;
    void*(*init)(void* context);
    void (*free)(void* context, void* instance);
    void (*seek)(void* context, void* instance, float sec);
    void (*rate)(void* context, void* instance, float factor);
    bool (*play)(void* context, void* instance, AudioSample* out, int samples);
} AudioSource;

void audio_init();
void audio_update(AudioSample* out, int samples);
void audio_stop(AudioInstance* instance);
void audio_pause(AudioInstance* instance);
void audio_resume(AudioInstance* instance);
void audio_seek(AudioInstance* instance, float sec);
void audio_rate(AudioInstance* instance, float factor);
void audio_play_oneshot(AudioSource* audio); // holy fucking shit niko oneshot 🥞🐱
AudioInstance* audio_play(AudioSource* audio);

void* loader_wav(uint8_t* bytes, int size);

#endif
