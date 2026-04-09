#depends "scripts/engine.c"

typedef struct Synthesizer {
    int16_t curr_sample;
    float rate, pos, phase;
    float start, length;
    float note_start, note_end;
    float vol_start, vol_end;
    void* osc;
} Synthesizer;

typedef struct Audio Audio;
Audio* audio;

float osc_polyblep(float t, float dt) {
    if (t < dt) {
        t /= dt;
        return -(t * t - 2 * t + 1);
    }
    else if (t > 1 - dt) {
        t = (t - 1) / dt;
        return t * t + 2 * t + 1;
    }
    return 0;
}

float osc_square(Synthesizer* synth, float t, float freq) {
    float dt = freq / 44100.f;
    float x = t < 0.5 ? -1 : 1;
    x -= osc_polyblep(t, dt);
    t += 0.5;
    x += osc_polyblep(t - (int)t, dt);
    return x;
}

float osc_saw(Synthesizer* synth, float t, float freq) {
    float dt = freq / 44100.f;
    float x = 2 * t - 1;
    x -= osc_polyblep(t, dt);
    return x;
}

float osc_triangle(Synthesizer* synth, float t, float freq) {
    return (2 * fabsf(2 * t - 1) - 1) * 1.25;
}

float osc_sine(Synthesizer* synth, float t, float freq) {
    return sinf(2 * 3.14159 * t);
}

float osc_noise(Synthesizer* synth, float t, float freq) {
    uint32_t x = floorf((synth.pos - t / freq) * 44100);
    x ^= x >> 17;
    x *= 0xbf324c81u;
    x ^= x >> 11;
    x *= 0x68bce3b9u;
    x ^= x >> 16;
    return (float)x / 4294967296.0f;
}

void play(Synthesizer* this) {
    if (!this.osc || this.pos < this.start || this.pos >= this.start + this.length) return;
    float(*osc)(struct Synthesizer*, float, float) = this.osc;
    float x = (this.pos - this.start) / this.length;
    float vol = (this.vol_end - this.vol_start) * x + this.vol_start;
    float note = (this.note_end - this.note_start) * x + this.note_start;
    float freq = 440 * powf(2, (note - 69) / 12);
    float sample = osc(this, this.phase, freq) * vol;
    this.phase += freq / 44100.f;
    this.phase -= (int)this.phase;
    this.curr_sample = sample * 32767 * 0.4;
}

Synthesizer* new_sample(Synthesizer* this) -> this.curr_sample = 0, this;

Synthesizer* oscillator(Synthesizer* this, float(*osc)(float), float length, float note) {
    this.play();
    this.start += this.length;
    this.length = length;
    this.note_start = this.note_end = note;
    this.vol_start = this.vol_end = 1;
    this.osc = osc;
    return this;
}

Synthesizer* silence(Synthesizer* this, float length) -> this.oscillator(nullptr, length, 0);
Synthesizer* square(Synthesizer* this, float length, float note) -> this.oscillator(osc_square, length, note);
Synthesizer* triangle(Synthesizer* this, float length, float note) -> this.oscillator(osc_triangle, length, note);
Synthesizer* saw(Synthesizer* this, float length, float note) -> this.oscillator(osc_saw, length, note);
Synthesizer* noise(Synthesizer* this, float length, float note) -> this.oscillator(osc_noise, length, note);
Synthesizer* sine(Synthesizer* this, float length, float note) -> this.oscillator(osc_sine, length, note);

Synthesizer* slide(Synthesizer* this, float slide) -> this.note_end = slide, this;
Synthesizer* volume(Synthesizer* this, float volume) -> this.vol_start = this.vol_end = volume, this;
Synthesizer* volume_slide(Synthesizer* this, float from, float to) -> this.vol_start = from, this.vol_end = to, this;

bool finish(Synthesizer* this) {
    this.play();
    return this.pos < this.start + this.length;
}

AudioSource** audio_sources;
int audio_num_sources, audio_cap_sources;

AudioSource* create(Audio* this, bool(*func)(Synthesizer*)) {
    for (int i = 0; i < audio_num_sources; i++) {
        if (audio_sources[i].context == func) return audio_sources[i];
    }

    AudioSource* source = malloc(sizeof(AudioSource));
    source.context = func;
    source.init = lambda syn_init(void* ctx): void* {
        Synthesizer* synth = calloc(sizeof(Synthesizer), 1);
        synth.rate = 1;
        return synth;
    };
    source.free = lambda syn_free(void* ctx, void* inst): void -> free(inst);
    source.rate = lambda syn_rate(void* ctx, void* inst, float rate): void -> ((Synthesizer*)inst).rate = rate;
    source.seek = lambda syn_rate(void* ctx, void* inst, float sec):  void -> ((Synthesizer*)inst).pos  = sec;
    source.play = lambda syn_play(void* ctx, void* inst, AudioSample* samples, int num_samples): bool {
        bool(*func)(Synthesizer*) = ctx;
        Synthesizer* synth = inst;
        for (int i = 0; i < num_samples; i++) {
            synth.start = 0;
            synth.length = 0;
            synth.osc = nullptr;
            if (!func(synth)) return false;
            samples[i] = synth.curr_sample;
            synth.pos += synth.rate / 44100.f;
        }
        return true;
    };

    if (audio_num_sources == audio_cap_sources) {
        if (!audio_cap_sources) audio_cap_sources = 4;
        else audio_cap_sources *= 2;
        audio_sources = realloc(audio_sources, sizeof(AudioSource*) * audio_cap_sources);
    }
    audio_sources[audio_num_sources++] = source;
    return source;
}

bool note_readint(const char** str, int* out) {
    char* end;
    *out = strtol(*str, &end, 10);
    if (*str == end) return false;
    *str = end;
    return true;
}

float note(const char* note) {
    int tone = 0;
    int octave = 4;
    int cents = 0;
    if (((*note) == 'C' || (*note) == 'c') && note++) {
        tone = 0;
        if ((*note) == '#' && note++) tone++;
    }
    else if (((*note) == 'D' || (*note) == 'd') && note++) {
        tone = 2;
        if ((*note) == '#' && note++) tone++;
        else if ((*note) == 'b' && note++) tone--;
    }
    else if (((*note) == 'E' || (*note) == 'e') && note++) {
        tone = 4;
        if ((*note) == 'b' && note++) tone--;
    }
    else if (((*note) == 'F' || (*note) == 'f') && note++) {
        tone = 5;
        if ((*note) == '#' && note++) tone++;
    }
    else if (((*note) == 'G' || (*note) == 'g') && note++) {
        tone = 7;
        if ((*note) == '#' && note++) tone++;
        else if ((*note) == 'b' && note++) tone--;
    }
    else if (((*note) == 'A' || (*note) == 'a') && note++) {
        tone = 9;
        if ((*note) == '#' && note++) tone++;
        else if ((*note) == 'b' && note++) tone--;
    }
    else if (((*note) == 'B' || (*note) == 'b' || (*note) == 'H' || (*note) == 'h') && note++) {
        tone = 11;
        if ((*note) == 'b' && note++) tone--;
    }
    else return nan("");
    if ((*note) >= '0' && (*note) <= '9') {
        if (!note_readint(&note, &octave)) return nan("");
    }
    if ((*note) == '+' || (*note) == '-') {
        int sign = (*note) == '-' ? -1 : 1; note++;
        if (!note_readint(&note, &cents)) return nan("");
        cents *= sign;
    }
    if (*note == 0) return (tone + (octave + 1) * 12) + (cents / 100.f);
    else return nan("");
}
