#ifndef ENGINE_H
#define ENGINE_H

#include "api/symbols.h"
#include "api/types.h"

#include "stdlib.h"

typedef struct {} Engine;
typedef struct {} Graphics;
typedef struct {} Assets;
typedef struct {} Input;
typedef Node*(*Level)();

typedef struct {
    Node* curr_node;
    int ptr;
} NodeBuilder;

preserve Engine* engine;
preserve Graphics* gfx;
preserve Assets* assets;
preserve Input* input;

#define NODE(type, ...) preserve int get_type(__ID__(type, Node)* this) -> __ID__(NodeType_, type);
#include "api/nodes.h"
#undef NODE

<T> T* new_node(Engine* this) {
    T* node = calloc(sizeof(T), 1);
    node.node.type = node.get_type();
    node.node.size = sizeof(T);
    return node;
}

preserve uint64_t get_micros(Engine* this) -> __get_micros();
preserve uint64_t get_millis(Engine* this) -> __get_millis();

preserve void attach(Node* this, Node* child) -> __engine_attach_node(this, child);
preserve void detach(Node* this) -> __engine_detach_node(this);
preserve void delete(Node* this) -> __engine_delete_node(this);
preserve void copy(Node* this) -> __engine_copy_node(this);
preserve uint8_t* tile(TilemapNode* this, int x, int y) -> __engine_tile(this, x, y);
<T> T* prop(EntityNode* this, const char* name) -> (T*)__engine_property(this, name);

preserve Window* get_main(Graphics* this) -> __graphics_main_window();
preserve Window* open(Graphics* this, const char* title, int width, int height) -> __graphics_open(title, width, height);
preserve void close(Window* this) -> __graphics_close(this);
preserve void focus(Window* this) -> __graphics_focus(this);
preserve void get_size(Window* this, int* width, int* height) -> __graphics_get_size(this, width, height);
preserve void get_pos(Window* this, int* x, int* y) -> __graphics_get_pos(this, x, y);
preserve void set_active(Window* this) -> __graphics_set_active(this);
preserve void start_frame(Window* this) -> __graphics_start_frame(this);
preserve void end_frame(Window* this) -> __graphics_end_frame(this);
preserve void post_process(Window* this, Shader* shader) -> __graphics_post_process(this, shader);
preserve void set_shader(Window* this, Shader* shader) -> __graphics_set_shader(this, shader);
preserve void rect(Window* this, float x, float y, float w, float h, int color) -> __graphics_rect(this, x, y, w, h, color);
preserve void draw(Window* this, Texture* texture, float x, float y, float w, float h, float sx, float sy, float sw, float sh, int color) -> __graphics_draw(this, texture, x, y, w, h, sx, sy, sw, sh, color);
preserve void blit(Window* this, Buffer* buffer, float x, float y, float w, float h, float sx, float sy, float sw, float sh, int color) -> __graphics_blit(this, buffer, x, y, w, h, sx, sy, sw, sh, color);
preserve Buffer* new_buffer(Window* this, int width, int height) -> __graphics_new_buffer(this, width, height);
preserve void set_buffer(Window* this, Buffer* buffer) -> __graphics_set_buffer(this, buffer);
preserve void destroy(Buffer* this) -> __graphics_destroy_buffer(this);
preserve bool should_close(Graphics* this) -> __graphics_should_close();

preserve void add_entry(Input* this, const char* name) -> __keybind_add_entry(name);
preserve void remove_entry(Input* this, const char* name) -> __keybind_remove_entry(name);
preserve void add(Input* this, const char* name, int keybind) -> __keybind_add(name, keybind);
preserve void remove(Input* this, const char* name, int keybind) -> __keybind_remove(name, keybind);
preserve int* get(Input* this, const char* name, int* count) -> __keybind_get(name, count);
preserve const char** get_entries(Input* this, int* count) -> __keybind_get_entries(count);
preserve bool down(Input* this, const char* name) -> __keybind_down(name);
preserve bool pressed(Input* this, const char* name) -> __keybind_pressed(name);
preserve bool released(Input* this, const char* name) -> __keybind_released(name);

<T> T* get(Assets* this, const char* name) -> __get_asset(name);

preserve NodeBuilder* breakpoint(NodeBuilder* this) {
    interrupt;
    return this;
}

preserve NodeBuilder* attach(NodeBuilder* this, Node* node) {
    this.curr_node.attach(node);
    return this;
}

preserve NodeBuilder* exec(NodeBuilder* this, void(*func)(NodeBuilder* builder)) {
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

preserve NodeBuilder* reopen(NodeBuilder* this) {
    if (this.curr_node.children_size == 0) return this;
    this.curr_node = this.curr_node.children[this.curr_node.children_size - 1];
    this.ptr = sizeof(Node);
    return this;
}

preserve NodeBuilder* close(NodeBuilder* this) {
    if (this.curr_node.parent == nullptr) return this;
    this.curr_node = this.curr_node.parent;
    this.ptr = sizeof(Node);
    return this;
}

preserve Node* build(NodeBuilder* this) {
    Node* node = this.curr_node;
    free(this);
    return node;
}

#endif
