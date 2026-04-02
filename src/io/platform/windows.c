#include <stdint.h>
#include <stdbool.h>

#include <windows.h>

uint64_t get_micros() {
    static LARGE_INTEGER freq = {};
    if (freq.QuadPart == 0)
        QueryPerformanceFrequency(&freq);
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (counter.QuadPart * 1000000) / freq.QuadPart;
}
