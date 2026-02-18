#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "math.h"

#ifndef __JITC__
#define __ID__(a, b) a##b
#endif

typedef enum {
#define NODE(type, ...) __ID__(NodeType_, type),
#include "headers/nodes.h"
#undef NODE
} NodeType;

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

typedef unsigned char Tile;

typedef int16_t AudioSample;
typedef struct AudioInstance AudioInstance;
typedef struct {
    void* context;
    void*(*init)(void* context);
    void (*free)(void* context, void* instance);
    void (*seek)(void* context, void* instance, float sec);
    void (*rate)(void* context, void* instance, float factor);
    bool (*play)(void* context, void* instance, AudioSample* out, int samples);
} AudioSource;

typedef enum {
    Direction_None,
    Direction_Up,
    Direction_Left,
    Direction_Down,
    Direction_Right,
} Direction;

typedef struct Node Node;
struct Node {
    NodeType type;
    int children_size, children_capacity;
    Node** children;
    Node* parent;
    int size;
};

#define _(type, name, parser) typeof(type) name;
#define NODE(type, ...) typedef struct { Node node; __VA_ARGS__ } __ID__(type, Node);
#include "headers/nodes.h"
#undef NODE
#undef _

typedef struct Shader Shader;
typedef struct Window Window;
typedef struct Buffer Buffer;

extern("engine_attach_node") void __engine_attach_node(Node* parent, Node* child);
extern("engine_detach_node") void __engine_detach_node(Node* child);
extern("engine_delete_node") void __engine_delete_node(Node* node);
extern("engine_deep_copy") Node* __engine_copy_node(Node* node);
extern("engine_tile") uint8_t* __engine_tile(TilemapNode* node, int x, int y);
extern("engine_property") void* __engine_property(EntityNode* node, const char* name);
extern("engine_find_entity") EntityNode* __engine_find_entity(LevelRootNode* level, const char* name);
extern("engine_find_entity_on_tilemap") EntityNode* __engine_find_entity_on_tilemap(TilemapNode* tilemap, const char* name);

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

typedef struct {} Engine;
typedef struct {} Graphics;
typedef struct {} Assets;
typedef struct {} Input;
typedef Node*(*Level)();

typedef struct {
    Node* curr_node;
    int ptr;
} NodeBuilder;

Engine* engine;
Graphics* gfx;
Assets* assets;
Input* input;

#define NODE(type, ...) int get_type(__ID__(type, Node)* this) -> __ID__(NodeType_, type);
#include "headers/nodes.h"
#undef NODE

<T> T* new_node(Engine* this) {
    T* node = calloc(sizeof(T), 1);
    node.node.type = node.get_type();
    node.node.size = sizeof(T);
    return node;
}

uint64_t get_micros(Engine* this) -> __get_micros();
uint64_t get_millis(Engine* this) -> __get_millis();

void attach(Node* this, Node* child) -> __engine_attach_node(this, child);
void detach(Node* this) -> __engine_detach_node(this);
void delete(Node* this) -> __engine_delete_node(this);
void copy(Node* this) -> __engine_copy_node(this);
uint8_t* tile(TilemapNode* this, int x, int y) -> __engine_tile(this, x, y);
<T> T* prop(EntityNode* this, const char* name) -> (T*)__engine_property(this, name);

EntityNode* find(LevelRootNode* this, const char* name) -> __engine_find_entity(this, name);
EntityNode* find(TilemapNode* this, const char* name) -> __engine_find_entity_on_tilemap(this, name);

Window* get_main(Graphics* this) -> __graphics_main_window();
Window* open(Graphics* this, const char* title, int width, int height) -> __graphics_open(title, width, height);
void close(Window* this) -> __graphics_close(this);
void focus(Window* this) -> __graphics_focus(this);
void get_size(Window* this, int* width, int* height) -> __graphics_get_size(this, width, height);
void get_pos(Window* this, int* x, int* y) -> __graphics_get_pos(this, x, y);
void set_active(Window* this) -> __graphics_set_active(this);
void start_frame(Window* this) -> __graphics_start_frame(this);
void end_frame(Window* this) -> __graphics_end_frame(this);
void post_process(Window* this, Shader* shader) -> __graphics_post_process(this, shader);
void set_shader(Window* this, Shader* shader) -> __graphics_set_shader(this, shader);
void rect(Window* this, float x, float y, float w, float h, int color) -> __graphics_rect(this, x, y, w, h, color);
void draw(Window* this, Texture* texture, float x, float y, float w, float h, float sx, float sy, float sw, float sh, int color) -> __graphics_draw(this, texture, x, y, w, h, sx, sy, sw, sh, color);
void blit(Window* this, Buffer* buffer, float x, float y, float w, float h, float sx, float sy, float sw, float sh, int color) -> __graphics_blit(this, buffer, x, y, w, h, sx, sy, sw, sh, color);
Buffer* new_buffer(Window* this, int width, int height) -> __graphics_new_buffer(this, width, height);
void set_buffer(Window* this, Buffer* buffer) -> __graphics_set_buffer(this, buffer);
void destroy(Buffer* this) -> __graphics_destroy_buffer(this);
bool should_close(Graphics* this) -> __graphics_should_close();

void add_entry(Input* this, const char* name) -> __keybind_add_entry(name);
void remove_entry(Input* this, const char* name) -> __keybind_remove_entry(name);
void add(Input* this, const char* name, int keybind) -> __keybind_add(name, keybind);
void remove(Input* this, const char* name, int keybind) -> __keybind_remove(name, keybind);
int* get(Input* this, const char* name, int* count) -> __keybind_get(name, count);
const char** get_entries(Input* this, int* count) -> __keybind_get_entries(count);
bool down(Input* this, const char* name) -> __keybind_down(name);
bool pressed(Input* this, const char* name) -> __keybind_pressed(name);
bool released(Input* this, const char* name) -> __keybind_released(name);

<T> T* get(Assets* this, const char* name) -> __get_asset(name);

NodeBuilder* breakpoint(NodeBuilder* this) {
    interrupt;
    return this;
}

NodeBuilder* attach(NodeBuilder* this, Node* node) {
    this.curr_node.attach(node);
    return this;
}

NodeBuilder* exec(NodeBuilder* this, void(*func)(NodeBuilder* builder)) {
    func(this);
    return this;
}

<T> NodeBuilder* open(Engine* this) {
    NodeBuilder* builder = calloc(sizeof(NodeBuilder), 1);
    builder.curr_node = engine.new_node<T>();
    builder.ptr = sizeof(Node);
    return builder;
}

<T> NodeBuilder* prop(NodeBuilder* this, T value) {
    if (this.ptr % alignof(T) != 0) this.ptr += alignof(T) - (this.ptr % alignof(T));
    *(T*)((char*)this.curr_node + this.ptr) = value;
    this.ptr += sizeof(T);
    return this;
}

<T> NodeBuilder* skip(NodeBuilder* this) {
    if (this.ptr % alignof(T) != 0) this.ptr += alignof(T) - (this.ptr % alignof(T));
    this.ptr += sizeof(T);
    return this;
}

<T> NodeBuilder* event(NodeBuilder* this, void* func) {
    return this.open<T>().prop<void*>(func).close();
}

<T> NodeBuilder* open(NodeBuilder* this) {
    T* child = engine.new_node<T>();
    this.curr_node.attach(child);
    this.curr_node = child;
    this.ptr = sizeof(Node);
    return this;
}

NodeBuilder* reopen(NodeBuilder* this) {
    if (this.curr_node.children_size == 0) return this;
    this.curr_node = this.curr_node.children[this.curr_node.children_size - 1];
    this.ptr = sizeof(Node);
    return this;
}

NodeBuilder* close(NodeBuilder* this) {
    if (this.curr_node.parent == nullptr) return this;
    this.curr_node = this.curr_node.parent;
    this.ptr = sizeof(Node);
    return this;
}

Node* build(NodeBuilder* this) {
    Node* node = this.curr_node;
    free(this);
    return node;
}

bool is(EntityNode* this, const char* name) {
    if (!this.name) return false;
    return strcmp(this.name, name) == 0;
}
