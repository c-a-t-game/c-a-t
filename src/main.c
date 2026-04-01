#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "storage.h"

#include "io/assets.h"
#include "io/graphics.h"
#include "io/input.h"
#include "io/audio.h"
#include "io/platform.h"

#include "engine/engine.h"
#include "jitc/jitc.h"

jitc_context_t* jitc_context;
static bool compilation_failed = false;
static bool editor_mode_enabled = false;

static void compile_progress(const char* curr_file, int total, int compiled) {
    char progress[51];
    memset(progress, 0, 51);
    memset(progress, '=', compiled * 50 / total);
    printf("%3d%% [%-50s] %s\n", compiled * 100 / total, progress, curr_file ?: "Done");
    fflush(stdout);
}

static void recompile_file(const char* file) {
    uint64_t start = get_micros();
    printf("Reloading '%s'...", file);
    if (!jitc_parse_file(jitc_context, file)) jitc_report_error(jitc_context, stdout);
    else printf("%.2f ms\n", (get_micros() - start) / 1000.f);
}

void add_compile_job(const char* code, const char* filename) {
    if (!jitc_append_task(jitc_context, code, filename)) {
        compilation_failed = true;
        jitc_report_error(jitc_context, stdout);
        return;
    }
    char path[sizeof("assets/") + strlen(filename)];
    strcpy(path, "assets/");
    strcat(path, filename);
    watch_file(strdup(path), recompile_file);
}

#ifdef _WIN32
#define main game_entrypoint

float fabsf(float x) {
    return x < 0 ? -x : x;
}

#endif

int main(int argc, char** argv) {
    if (argc > 1) {
        if (strcmp(argv[1], "--editor") == 0) editor_mode_enabled = true;
    }

    jitc_context = jitc_create_context();
    load_assets();
    storage_init();
    audio_init();
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

    return 0;
}

bool check_editor_mode() {
    return editor_mode_enabled;
}
