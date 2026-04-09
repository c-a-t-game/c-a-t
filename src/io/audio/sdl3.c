#include "io/audio.h"

#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>

FILE* wav;
int samples_written = 0;

typedef struct wav_header {
    // RIFF Header
    char riff_header[4]; // Contains "RIFF"
    int wav_size; // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
    char wave_header[4]; // Contains "WAVE"

    // Format Header
    char fmt_header[4]; // Contains "fmt " (includes trailing space)
    int fmt_chunk_size; // Should be 16 for PCM
    short audio_format; // Should be 1 for PCM. 3 for IEEE Float
    short num_channels;
    int sample_rate;
    int byte_rate; // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
    short sample_alignment; // num_channels * Bytes Per Sample
    short bit_depth; // Number of bits per sample

    // Data
    char data_header[4]; // Contains "data"
    int data_bytes; // Number of bytes in data. Number of samples * num_channels * sample byte size
    // uint8_t bytes[]; // Remainder of wave file is bytes
} wav_header;

void finalize_wav() {
    fseek(wav, 0, SEEK_SET);
    fwrite(&(struct wav_header){
        .riff_header = "RIFF",
        .wav_size = samples_written * sizeof(AudioSample) + sizeof(wav_header) - 8,
        .wave_header = "WAVE",
        .fmt_header = "fmt ",
        .fmt_chunk_size = 16,
        .audio_format = 1,
        .num_channels = 1,
        .sample_rate = 44100,
        .byte_rate = 44100 * sizeof(AudioSample),
        .sample_alignment = sizeof(AudioSample),
        .bit_depth = sizeof(AudioSample) * 8,
        .data_header = "data",
        .data_bytes = samples_written * sizeof(AudioSample)
    }, sizeof(wav_header), 1, wav);
    fclose(wav);
}

void audio_provider(void* data, SDL_AudioStream* stream, int bytes, int total_bytes) {
    if (bytes == 0) return;
    int samples = bytes / sizeof(AudioSample);
    AudioSample buf[samples];
    audio_update(buf, samples);
    SDL_PutAudioStreamData(stream, buf, bytes);
    fwrite(buf, samples, sizeof(AudioSample), wav);
    samples_written += samples;
}

void audio_init() {
    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioSpec spec = { SDL_AUDIO_S16, 1, SAMPLE_RATE };
    SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    SDL_SetAudioStreamGetCallback(stream, audio_provider, NULL);
    SDL_ResumeAudioStreamDevice(stream);
    wav = fopen("test.wav", "w");
    fwrite(&(struct wav_header){}, sizeof(wav_header), 1, wav);
    atexit(finalize_wav);
}
