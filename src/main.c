#include <stdio.h>

#include "io/assets.h"
#include "io/graphics.h"

int main() {
    load_assets();
    Window* window = graphics_open(":3", 768, 512);
    graphics_set_active(window);
    while (!graphics_should_close()) {
        graphics_start_frame(NULL);
        Buffer* buffer = graphics_new_buffer(NULL, 384, 256);
        graphics_set_buffer(NULL, buffer);
        graphics_draw(NULL, get_asset(Texture, "trol.png"), 64, 64, 64, 64, 0, 0, 64, 64, GRAY(255));
        graphics_set_buffer(NULL, NULL);
        graphics_blit(NULL, buffer, 0, 0, 768, 512, 0, 0, 384, 256, GRAY(255));
        graphics_end_frame(NULL);
        graphics_destroy_buffer(buffer);
    }
    graphics_close(window);
    return 0;
}
