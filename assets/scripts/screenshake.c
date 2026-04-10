#depends "scripts/random.c"

float screenshake = 0;

void process_screenshake(float* x, float* y, float dt) {
    float ss_x = frng(-1, 1);
    float ss_y = frng(-1, 1);
    *x = ss_x * screenshake / 4;
    *y = ss_y * screenshake / 4;
    screenshake -= dt;
    if (screenshake < 0) screenshake = 0;
}
