#include "io/audio.h"

#include <SDL3/SDL.h>

void audio_provider(void* data, SDL_AudioStream* stream, int bytes, int total_bytes) {
    if (bytes == 0) return;
    int samples = bytes / sizeof(AudioSample);
    AudioSample buf[samples];
    audio_update(buf, samples);
    SDL_PutAudioStreamData(stream, buf, bytes);
}

void audio_init() {
    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioSpec spec = { SDL_AUDIO_S16, 1, SAMPLE_RATE };
    SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    SDL_SetAudioStreamGetCallback(stream, audio_provider, NULL);
    SDL_ResumeAudioStreamDevice(stream);
}
