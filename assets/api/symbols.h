#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "api/types.h"

#ifndef __JITC__
#define extern(symbol) extern
#endif

#define COLOR(x) (uint8_t)((x) * 255)
#define GRAY(x) GRAYA(x, 1.0)
#define GRAYA(x, a) RGBA(x, x, x, a)
#define RGB(r, g, b) RGBA(r, g, b, 1.0)
#define HSV(h, s, v) HSVA(h, s, v, 1.0)
#define RGBA(R, G, B, A) (Color){ .r = COLOR(R), .g = COLOR(G), .b = COLOR(B), .a = COLOR(A) }
#define HSVA(h, s, v, a) HSV_TO_RGB((float)(h), (float)(s), (float)(v), a)
#define HSV_TO_RGB(h, s, v, a) ( \
    ((int)((h) * 6) % 6) == 0 ? RGBA((v), (v) * (1 - (1 - ((h) * 6 - 0)) * (s)), (v) * (1 - (s)), (a)) : \
    ((int)((h) * 6) % 6) == 1 ? RGBA((v) * (1 - ((h) * 6 - 1) * (s)), (v), (v) * (1 - (s)), (a)) : \
    ((int)((h) * 6) % 6) == 2 ? RGBA((v) * (1 - (s)), (v), (v) * (1 - (1 - ((h) * 6 - 2)) * (s)), (a)) : \
    ((int)((h) * 6) % 6) == 3 ? RGBA((v) * (1 - (s)), (v) * (1 - ((h) * 6 - 3) * (s)), (v), (a)) : \
    ((int)((h) * 6) % 6) == 4 ? RGBA((v) * (1 - (1 - ((h) * 6 - 4)) * (s)), (v) * (1 - (s)), (v), (a)) : \
    ((int)((h) * 6) % 6) == 5 ? RGBA((v), (v) * (1 - (s)), (v) * (1 - ((h) * 6 - 5) * (s)), (a)) : 0 \
)

extern("engine_attach_node") void __engine_attach_node(Node* parent, Node* child);
extern("engine_detach_node") void __engine_detach_node(Node* child);
extern("engine_delete_node") void __engine_delete_node(Node* node);
extern("engine_deep_copy") Node* __engine_copy_node(Node* node);
extern("engine_tile") uint8_t* __engine_tile(TilemapNode* node, int x, int y);
extern("engine_property") void* __engine_property(EntityNode* node, const char* name);

extern("graphics_main_window") Window* __graphics_main_window();
extern("graphics_open") Window* __graphics_open(const char* title, int width, int height);
extern("graphics_close") void __graphics_close(Window* window);
extern("graphics_focus") void __graphics_focus(Window* window);
extern("graphics_get_size") void __graphics_get_size(Window* window, int* width, int* height);
extern("graphics_get_pos") void __graphics_get_pos(Window* window, int* x, int* y);
extern("graphics_set_active") void __graphics_set_active(Window* window);
extern("graphics_start_frame") void __graphics_start_frame(Window* window);
extern("graphics_end_frame") void __graphics_end_frame(Window* window);
extern("graphics_post_process") void __graphics_post_process(Window* window, Shader* shader);
extern("graphics_set_shader") void __graphics_set_shader(Window* window, Shader* shader);
extern("graphics_rect") void __graphics_rect(Window* window, float x, float y, float w, float h, int color);
extern("graphics_draw") void __graphics_draw(Window* window, Texture* texture, float x, float y, float w, float h, float sx, float sy, float sw, float sh, int color);
extern("graphics_blit") void __graphics_blit(Window* window, Buffer* buffer,  float x, float y, float w, float h, float sx, float sy, float sw, float sh, int color);
extern("graphics_new_buffer") Buffer* __graphics_new_buffer(Window* window, int width, int height);
extern("graphics_set_buffer") void __graphics_set_buffer(Window* window, Buffer* buffer);
extern("graphics_destroy_buffer") void __graphics_destroy_buffer(Buffer* buffer);
extern("graphics_should_close") bool __graphics_should_close();

extern("keybind_add_entry") void __keybind_add_entry(const char* name);
extern("keybind_remove_entry") void __keybind_remove_entry(const char* name);
extern("keybind_add") void __keybind_add(const char* name, int keybind);
extern("keybind_remove") void __keybind_remove(const char* name, int keybind);
extern("keybind_get") int* __keybind_get(const char* name, int* count);
extern("keybind_get_entries") const char** __keybind_get_entries(int* count);
extern("keybind_down") bool __keybind_down(const char* name);
extern("keybind_pressed") bool __keybind_pressed(const char* name);
extern("keybind_released") bool __keybind_released(const char* name);

extern("_get_asset") void* __get_asset(const char* name);

extern("get_millis") uint64_t __get_millis();
extern("get_micros") uint64_t __get_micros();

extern LevelRootNode* current_level;

#endif
