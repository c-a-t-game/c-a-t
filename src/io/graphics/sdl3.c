#include <SDL3/SDL.h>

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <stdlib.h>

#include "io/graphics.h"
#include "stb_image.h"

typedef struct {
    Texture* texture;
    SDL_Texture* handle;
} TextureEntry;

struct Window {
    SDL_Window* wnd;
    SDL_Renderer* rnd;
    struct {
        TextureEntry* entries;
        int size, capacity;
    } texture_map;
};

static Window* curr_window = NULL;

static int compare_entry(const void* _a, const void* _b) {
    TextureEntry* a = *(TextureEntry**)_a;
    TextureEntry* b = *(TextureEntry**)_b;
    return (uintptr_t)b->texture - (uintptr_t)a->texture;
}

static SDL_Texture* get_texture(Window* window, Texture* texture, SDL_Renderer* renderer) {
    TextureEntry* entry = bsearch(&texture, window->texture_map.entries, window->texture_map.size, sizeof(TextureEntry), compare_entry);
    if (entry) return entry->handle;
    if (window->texture_map.size == window->texture_map.capacity) {
        window->texture_map.capacity *= 2;
        window->texture_map.entries = realloc(window->texture_map.entries, sizeof(TextureEntry) * window->texture_map.capacity);
    }
    SDL_Surface* surface = SDL_CreateSurfaceFrom(texture->width, texture->height, SDL_PIXELFORMAT_ABGR8888, texture->colors, 4 * texture->width);
    SDL_Texture* handle = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetTextureScaleMode(handle, SDL_SCALEMODE_NEAREST);
    SDL_DestroySurface(surface);
    window->texture_map.entries[window->texture_map.size++] = (TextureEntry){ .texture = texture, .handle = handle };
    qsort(window->texture_map.entries, window->texture_map.size, sizeof(TextureEntry), compare_entry);
    return handle;
}

SDL_Renderer* graphics_get_renderer(SDL_Window* window) {
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderVSync(renderer, 1);
    return renderer;
}

void graphics_destroy_renderer(SDL_Renderer* renderer) {
    return SDL_DestroyRenderer(renderer);
}

Window* graphics_open(const char* title, int width, int height) {
    static bool initialized = false;
    if (!initialized) SDL_Init(SDL_INIT_VIDEO);
    initialized = true;

    Window* w = malloc(sizeof(Window));
    w->wnd = SDL_CreateWindow(title, width, height, 0);
    w->rnd = graphics_get_renderer(w->wnd);
    w->texture_map.capacity = 4;
    w->texture_map.size = 0;
    w->texture_map.entries = malloc(sizeof(TextureEntry) * w->texture_map.capacity);
    curr_window = w;
    return w;
}

void graphics_close(Window* w) {
    if (!w) w = curr_window;
    graphics_destroy_renderer(w->rnd);
    SDL_DestroyWindow(w->wnd);
}

void graphics_focus(Window* w) {
    if (!w) w = curr_window;
    SDL_RaiseWindow(w->wnd);
}

void graphics_get_size(Window* w, int* width, int* height) {
    if (!w) w = curr_window;
    SDL_GetWindowSize(w->wnd, width, height);
}

void graphics_get_pos(Window* w, int* x, int* y) {
    if (!w) w = curr_window;
    SDL_GetWindowPosition(w->wnd, x, y);
}

void graphics_set_active(Window* w) {
    curr_window = w;
}

void graphics_start_frame(Window* w) {
    if (!w) w = curr_window;
    SDL_SetRenderDrawColor(w->rnd, 0, 0, 0, 255);
    SDL_RenderClear(w->rnd);
}

void graphics_end_frame(Window* w) {
    if (!w) w = curr_window;
    SDL_RenderPresent(w->rnd);
}

void graphics_rect(Window* window, float x, float y, float w, float h, Color color) {
    if (!window) window = curr_window;
    SDL_SetRenderDrawColor(window->rnd, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(window->rnd, (SDL_FRect[]){{ .x = x, .y = y, .w = w, .h = h }});
}

void graphics_draw(Window* window, Texture* texture, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, Color color) {
    if (!window) window = curr_window;
    void* tex = get_texture(window, texture, window->rnd);
    graphics_blit(window, tex, dx, dy, dw, dh, sx, sy, sw, sh, color);
}

void graphics_blit(Window* window, Buffer* buffer, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, Color color) {
    if (!window) window = curr_window;
    SDL_Texture* texture = (void*)buffer;
    SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(texture, color.a);
    SDL_RenderTexture(window->rnd, texture,
        (SDL_FRect[]){{ .x = sx, .y = sy, .w = sw, .h = sh }},
        (SDL_FRect[]){{ .x = dx, .y = dy, .w = dw, .h = dh }}
    );
}

Buffer* graphics_new_buffer(Window* window, int width, int height) {
    if (!window) window = curr_window;
    SDL_Texture* texture = SDL_CreateTexture(window->rnd, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, width, height);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    return (void*)texture;
}

void graphics_set_buffer(Window* window, Buffer* buffer) {
    if (!window) window = curr_window;
    SDL_SetRenderTarget(window->rnd, (void*)buffer);
}

void graphics_destroy_buffer(Buffer* buffer) {
    SDL_DestroyTexture((void*)buffer);
}

void* loader_png(uint8_t* data, int len) {
    int c;
    Texture* texture = malloc(sizeof(Texture));
    texture->colors = (Color*)stbi_load_from_memory(data, len, &texture->width, &texture->height, &c, 4);
    return texture;
}

void graphics_post_process(Window* w, Shader* shader) {}
void graphics_set_shader(Window* w, Shader* shader) {}
void* loader_glsl(uint8_t* data, int len) {
    return NULL;
}

bool graphics_should_close() {
    SDL_PumpEvents();
    return SDL_PeepEvents(NULL, 0, SDL_PEEKEVENT, SDL_EVENT_QUIT, SDL_EVENT_QUIT) > 0;
}
