#include "core/storage.h"

#include "core/assets/assets.h"
#include "core/io/graphics.h"
#include "core/io/input.h"
#include "core/io/audio.h"
#include "core/io/platform.h"

#include "core/engine/engine.h"
#include "core/scripts.h"

LevelRootNode* current_level;

int main() {
    scripts_init();
    load_assets();
    storage_init();
    if (!scripts_compile()) return 1;
    void(*entry_point)() = scripts_get("entry_point");
    if (!entry_point) return 1;
    entry_point();

    Window* window = graphics_open(":3", 1152, 768);
    graphics_set_active(window);
    audio_init();

    uint64_t last_micros = get_micros();
    while (!graphics_should_close()) {
        uint64_t curr_micros = get_micros();
        float delta_time = (curr_micros - last_micros - scripts_compile_time()) / 1000000.f * 60;
        last_micros = curr_micros;

        graphics_start_frame(NULL);
        Buffer* buffer = graphics_new_buffer(NULL, 384, 256);
        graphics_set_buffer(NULL, buffer);

        keybind_update();
        engine_update(current_level, delta_time);
        engine_render(current_level, 1152, 768);
        engine_cleanup();

        graphics_set_buffer(NULL, NULL);
        graphics_blit(NULL, buffer, 0, 0, 1152, 768, 0, 0, 384, 256, GRAY(255));
        graphics_end_frame(NULL);
        graphics_destroy_buffer(buffer);

        check_watched_files();
    }
    graphics_close(window);
    return 0;
}
