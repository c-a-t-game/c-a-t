#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <sys/time.h>
#include <sys/inotify.h>
#include <unistd.h>

#include "io/platform.h"

typedef struct {
    int wd;
    FileWatchCallback callback;
    const char* name;
} FileWatch;

static int watching_size, watching_capacity;
static FileWatch* watching;

static int inotify_fd = -1;

static int compare_int(const void* a, const void* b) {
    return *(int*)b - *(int*)a;
}

uint64_t get_micros() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

void watch_file(const char* filename, FileWatchCallback callback) {
    if (inotify_fd == -1) inotify_fd = inotify_init1(IN_NONBLOCK);
    int wd = inotify_add_watch(inotify_fd, filename, IN_MODIFY | IN_DONT_FOLLOW);
    if (wd < 0) return;
    if (watching_size == watching_capacity) {
        if (watching_capacity == 0) watching_capacity = 4;
        else watching_capacity *= 2;
        watching = realloc(watching, sizeof(FileWatch) * watching_capacity);
    }
    watching[watching_size++] = (FileWatch){
        .wd = wd,
        .callback = callback,
        .name = filename,
    };
    qsort(watching, watching_size, sizeof(FileWatch), compare_int);
}

void check_watched_files() {
    struct inotify_event event;
    int len = read(inotify_fd, &event, sizeof(struct inotify_event));
    if (len > 0) {
        FileWatch* watch = bsearch(&event.wd, watching, watching_size, sizeof(FileWatch), compare_int);
        if (event.mask & IN_IGNORED) {
            watch->wd = inotify_add_watch(inotify_fd, watch->name, IN_MODIFY | IN_DONT_FOLLOW);
            qsort(watching, watching_size, sizeof(FileWatch), compare_int);
        }
        else if (event.mask & IN_MODIFY) {
            if (watch->callback) watch->callback(watch->name);
        }
    }
}
