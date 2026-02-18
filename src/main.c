#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

#include "io/assets.h"
#include "io/graphics.h"
#include "io/input.h"
#include "io/audio.h"
#include "io/platform.h"

#include "engine/engine.h"
#include "jitc/jitc.h"

LevelRootNode* current_level;
jitc_context_t* jitc_context;
bool compilation_failed = false;

void add_compile_job(const char* code, const char* filename) {
    if (jitc_append_task(jitc_context, code, filename)) return;
    compilation_failed = true;
    jitc_report_error(jitc_context, stderr);
}

void compile_progress(const char* curr_file, int total, int compiled) {
    char progress[51];
    memset(progress, 0, 51);
    memset(progress, '=', compiled * 50 / total);
    printf("%3d%% [%-50s] %s\n", compiled * 100 / total, progress, curr_file ?: "Done");
}

int main() {
    jitc_context = jitc_create_context();
    load_assets();
    if (compilation_failed) return 1;
    if (!jitc_build(jitc_context, compile_progress)) {
        jitc_report_error(jitc_context, stderr);
        return 1;
    }

    void(*entry_point)() = jitc_get(jitc_context, "entry_point");
    if (!entry_point) {
        jitc_report_error(jitc_context, stderr);
        return 1;
    }
    entry_point();

    Window* window = graphics_open(":3", 1152, 768);
    graphics_set_active(window);
    audio_init();

    uint64_t last_micros = get_micros();
    while (!graphics_should_close()) {
        uint64_t curr_micros = get_micros();
        float delta_time = (curr_micros - last_micros) / 1000000.f * 60;
        last_micros = curr_micros;

        graphics_start_frame(NULL);
        Buffer* buffer = graphics_new_buffer(NULL, 384, 256);
        graphics_set_buffer(NULL, buffer);

        keybind_update();
        engine_update(current_level, delta_time);
        engine_render(current_level, 1152, 768);

        graphics_set_buffer(NULL, NULL);
        graphics_blit(NULL, buffer, 0, 0, 1152, 768, 0, 0, 384, 256, GRAY(255));
        graphics_end_frame(NULL);
        graphics_destroy_buffer(buffer);
    }
    graphics_close(window);
    return 0;
}
