#include "scripts.h"

#include "jitc/jitc.h"
#include "io/platform.h"

#include <string.h>
#include <stdint.h>

static jitc_context_t* context;
static bool compilation_failed;
static uint64_t compile_time;

static void compile_progress(const char* curr_file, int total, int compiled) {
    char progress[51];
    memset(progress, 0, 51);
    memset(progress, '=', compiled * 50 / total);
    printf("%3d%% [%-50s] %s\n", compiled * 100 / total, progress, curr_file ?: "Done");
}

static void recompile_file(const char* file) {
    uint64_t start = get_micros();
    printf("Reloading '%s'...", file);
    if (!jitc_parse_file(context, file)) jitc_report_error(context, stdout);
    else printf("%.2f ms\n", (get_micros() - start) / 1000.f);
    compile_time += get_micros() - start;
}

void scripts_add(const char* code, const char* filename) {
    if (!jitc_append_task(context, code, filename)) {
        compilation_failed = true;
        jitc_report_error(context, stdout);
        return;
    }
    char path[sizeof("assets/") + strlen(filename)];
    strcpy(path, "assets/");
    strcat(path, filename);
    watch_file(strdup(path), recompile_file);
}

void scripts_header(const char* filename, const char* data) {
    jitc_create_header(context, filename, data);
}

bool scripts_compile() {
    if (jitc_build(context, compile_progress)) return true;
    jitc_report_error(context, stderr);
    return false;
}

void* scripts_get(const char* symbol) {
    void* ptr;
    if ((ptr = jitc_get(context, symbol))) return ptr;
    jitc_report_error(context, stderr);
    return NULL;
}

uint64_t scripts_compile_time() {
    uint64_t comptime = compile_time;
    compile_time = 0;
    return comptime;
}

void scripts_init() {
    context = jitc_create_context();
}