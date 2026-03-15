#include "io/graphics.h"

#include <stddef.h>

static Window* main_window;

Window* graphics_open(const char* title, int width, int height) { return NULL; }
Window* graphics_main_window() { return main_window; }
void graphics_set_main(Window* w) { main_window = w; }
void graphics_close(Window* w) {}
void graphics_focus(Window* w) {}
void graphics_get_size(Window* w, int* width, int* height) {}
void graphics_get_pos(Window* w, int* x, int* y) {}
void graphics_set_active(Window* w) {}
void graphics_start_frame(Window* w) {}
void graphics_end_frame(Window* w) {}
void graphics_rect(Window* window, float x, float y, float w, float h, Color color) {}
void graphics_draw(Window* window, Texture* texture, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, Color color) {}
void graphics_blit(Window* window, Buffer* buffer, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, Color color) {}
Buffer* graphics_new_buffer(Window* window, int width, int height) { return NULL; }
void graphics_set_buffer(Window* window, Buffer* buffer) {}
void graphics_destroy_buffer(Buffer* buffer) {}
void* loader_png(const char* filename, uint8_t* data, int len) { return NULL; }
void graphics_post_process(Window* w, Shader* shader) {}
void graphics_set_shader(Window* w, Shader* shader) {}
void* loader_glsl(const char* filename, uint8_t* data, int len) { return NULL; }
bool graphics_should_close() { return true; }
