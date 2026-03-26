#include <stdlib.h>

int rng(int min, int max) {
    if (min > max) {
        int tmp = min;
        min = max;
        max = tmp;
    }
    return rand() % (max - min + 1) + min;
}

double frng(double min, double max) {
    if (min > max) {
        double tmp = min;
        min = max;
        max = tmp;
    }
    return rand() / (double)RAND_MAX * (max - min) + min;
}
