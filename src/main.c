#include <stdio.h>
#include <stdlib.h>

#include "main.h"

#include "io/assets.h"
#include "io/graphics.h"
#include "io/input.h"
#include "io/audio.h"

#include "engine/engine.h"
#include "jitc/jitc.h"

LevelRootNode* current_level;
jitc_context_t* jitc_context;

int compile_jobs_size, compile_jobs_capacity;
const char** compile_jobs;

void add_compile_job(const char* filename) {
    if (compile_jobs_size == compile_jobs_capacity) {
        if (compile_jobs_capacity == 0) compile_jobs_capacity = 4;
        else compile_jobs_capacity *= 2;
        compile_jobs = realloc(compile_jobs, sizeof(char*) * compile_jobs_capacity);
    }
    compile_jobs[compile_jobs_size++] = filename;
}

int main() {
    jitc_context = jitc_create_context();
    load_assets();
    for (int i = 0; i < compile_jobs_size; i++) {
        printf("Compiling '%s'\n", compile_jobs[i]);
        if (!jitc_parse(jitc_context, get_asset(char, compile_jobs[i]), compile_jobs[i])) {
            jitc_report_error(jitc_context, stderr);
            return 1;
        }
    }
    printf("Compiled\n");

    void(*entry_point)() = jitc_get(jitc_context, "entry_point");
    if (!entry_point) {
        jitc_report_error(jitc_context, stderr);
        return 1;
    }
    entry_point();

    audio_play_oneshot(get_asset(AudioSource, "RECONSTRUCTWHAT.wav"));
    Window* window = graphics_open(":3", 1152, 768);
    graphics_set_active(window);
    audio_init();

    while (!graphics_should_close()) {
        graphics_start_frame(NULL);
        Buffer* buffer = graphics_new_buffer(NULL, 384, 256);
        graphics_set_buffer(NULL, buffer);

        keybind_update();
        engine_update(current_level, 1);
        engine_render(current_level, 1152, 768);

        graphics_set_buffer(NULL, NULL);
        graphics_blit(NULL, buffer, 0, 0, 1152, 768, 0, 0, 384, 256, GRAY(255));
        graphics_end_frame(NULL);
        graphics_destroy_buffer(buffer);
    }
    graphics_close(window);
    return 0;
}
