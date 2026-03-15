#include <stdint.h>

#include <windows.h>

uint64_t get_micros() {
    static const uint64_t EPOCH = 116444736000000000ULL;
    SYSTEMTIME system_time; FILETIME file_time;
    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    uint64_t time = file_time.dwLowDateTime + ((uint64_t)file_time.dwHighDateTime << 32);
    return time - EPOCH;
}
