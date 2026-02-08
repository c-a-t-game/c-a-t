#include <stddef.h>
#include <stdint.h>

#include <sys/time.h>

uint64_t get_micros() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}
