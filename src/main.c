#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
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

static Window* window;

jitc_context_t* jitc_context;
static bool compilation_failed = false;
static bool editor_mode_enabled = false;

static void draw_text(Window* window, float anchor, int x, int y, Color color, const char* fmt, ...) {
    char *string, *ptr;
    va_list args;
    va_start(args, fmt);
    vasprintf(&string, fmt, args);
    va_end(args);
    ptr = string;

    char c;
    int pos_x = x - strlen(string) * 7 * anchor, pos_y = y;
    Texture* texture = get_asset(Texture, "images/hud/font.png");
    while ((c = *ptr++)) {
        int x = c % 16;
        int y = c / 16 - 2;
        graphics_draw(window, texture, pos_x, pos_y, 6, 8, x * 6, y * 8, 6, 8, color);
        pos_x += 7;
    }
    free(string);
}

static void compile_progress(const char* curr_file, int total, int compiled) {
    graphics_start_frame(window);
    float percent = compiled / (float)total;
    graphics_rect(window, 0, 0, 256, 64, GRAY(32));
    draw_text(window, 0.5, 128, 4, GRAY(255), "Compiling scripts...");
    draw_text(window, 0.0, 4, 44, GRAY(255), curr_file ?: "Done");
    draw_text(window, 1.0, 252, 44, GRAY(255), "%3d%%", compiled * 100 / total);
    graphics_rect(window, 0, 56, compiled * 256.f / total, 8, RGB(0, 192, 0));
    graphics_end_frame(window);
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

    window = graphics_open("Compile Progress", 256, 64);
    if (!jitc_build(jitc_context, compile_progress)) {
        jitc_report_error(jitc_context, stderr);
        return 1;
    }
    graphics_close(window);

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
