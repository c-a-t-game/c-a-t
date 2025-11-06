#include <stdio.h>

#include "io/assets.h"
#include "io/graphics.h"

int main() {
    load_assets();
    Window* window = graphics_open(":3", 768, 512);
    graphics_set_active(window);
    while (1) {
        if (graphics_should_close()) break;
        graphics_start_frame(NULL);
        graphics_end_frame(NULL);
    }
    graphics_close(window);
    return 0;
}
