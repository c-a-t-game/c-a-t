#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>

typedef union {
    struct {
        uint8_t a, b, g, r;
    };
    uint32_t data;
    float packed_float;
} Color;

typedef struct {
    Color* colors;
    int width, height;
} Texture;

typedef struct Shader Shader;
typedef struct Window Window;
typedef struct Buffer Buffer;

#define COLOR(x) (uint8_t)(_Generic((x), float: (x) * 255, double: (x) * 255, long double: (x) * 255, default: (x)))
#define GRAY(x) GRAYA(x, 255)
#define GRAYA(x, a) RGBA(x, x, x, a)
#define RGB(r, g, b) RGBA(r, g, b, 255)
#define HSV(h, s, v) HSVA(h, s, v, 255)
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

Window* graphics_open(const char* title, int width, int height);
void graphics_close(Window* window);
void graphics_focus(Window* window);
void graphics_get_size(Window* window, int* width, int* height);
void graphics_get_pos(Window* window, int* x, int* y);
void graphics_set_active(Window* window);
void graphics_start_frame(Window* window);
void graphics_end_frame(Window* window);
void graphics_post_process(Window* window, Shader* shader);
void graphics_set_shader(Window* window, Shader* shader);
void graphics_rect(Window* window, float x, float y, float w, float h, Color color);
void graphics_draw(Window* window, Texture* texture, float x, float y, float w, float h, float sx, float sy, float sw, float sh, Color color);
void graphics_blit(Window* window,  Buffer* buffer,  float x, float y, float w, float h, float sx, float sy, float sw, float sh, Color color);
Buffer* graphics_new_buffer(Window* window, int width, int height);
void graphics_set_buffer(Window* window, Buffer* buffer);
void graphics_destroy_buffer(Buffer* buffer);
bool graphics_should_close();

#endif
