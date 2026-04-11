#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

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

typedef enum {
    Collision_None,
    Collision_TopOnly,
    Collision_Solid,
} Collision;

typedef enum {
    MouseButton_Left   = 1 << 0,
    MouseButton_Middle = 1 << 1,
    MouseButton_Right  = 1 << 2,
} MouseButton;

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
typedef struct Engine Engine;
typedef struct Graphics Graphics;
typedef struct Assets Assets;
typedef struct Input Input;
typedef struct Storage Storage;
typedef struct StorageSlot StorageSlot;

typedef Node*(*Level)();
typedef void(*FileWatchCallback)(const char* filename);

extern("engine_attach_node") void __engine_attach_node(Node* parent, Node* child);
extern("engine_detach_node") void __engine_detach_node(Node* child);
extern("engine_delete_node") void __engine_delete_node(Node* node);
extern("engine_deep_copy") Node* __engine_copy_node(Node* node);
extern("engine_set_tile") void __engine_set_tile(TilemapNode* node, int x, int y, Tile tile);
extern("engine_get_tile") uint8_t __engine_get_tile(TilemapNode* node, int x, int y);
extern("engine_property") void* __engine_property(EntityNode* node, const char* name);
extern("engine_find_entity") EntityNode* __engine_find_entity(LevelRootNode* level, const char* name);
extern("engine_find_entity_on_tilemap") EntityNode* __engine_find_entity_on_tilemap(TilemapNode* tilemap, const char* name);
extern("engine_update") void __engine_update(LevelRootNode* node, float delta_time);
extern("engine_render") void __engine_render(LevelRootNode* node, float width, float height);
extern("engine_cleanup") void __engine_cleanup();
extern("engine_get_tileset") TilesetNode* __engine_get_tileset(TilemapNode* tilemap);

extern("graphics_open") Window* __graphics_open(const char* title, int width, int height);
extern("graphics_close") void __graphics_close(Window* window);
extern("graphics_focus") void __graphics_focus(Window* window);
extern("graphics_get_size") void __graphics_get_size(Window* window, int* width, int* height);
extern("graphics_get_pos") void __graphics_get_pos(Window* window, int* x, int* y);
extern("graphics_screen_size") void __graphics_screen_size(int* x, int* y);
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

extern("audio_stop") void __audio_stop(AudioInstance* instance);
extern("audio_pause") void __audio_pause(AudioInstance* instance);
extern("audio_resume") void __audio_resume(AudioInstance* instance);
extern("audio_seek") void __audio_seek(AudioInstance* instance, float sec);
extern("audio_rate") void __audio_rate(AudioInstance* instance, float factor);
extern("audio_volume") void __audio_volume(AudioInstance* instance, float volume);
extern("audio_is_paused") bool __audio_is_paused(AudioInstance* instance);
extern("audio_is_playing") bool __audio_is_playing(AudioInstance* instance);
extern("audio_is_done") bool __audio_is_done(AudioInstance* instance);
extern("audio_get_volume") float __audio_get_volume(AudioInstance* instance);
extern("audio_play_oneshot") void __audio_play_oneshot(AudioSource* audio);
extern("audio_play") AudioInstance* __audio_play(AudioSource* audio);

extern("keybind_add_entry") void __keybind_add_entry(const char* name);
extern("keybind_remove_entry") void __keybind_remove_entry(const char* name);
extern("keybind_add") void __keybind_add(const char* name, int keybind);
extern("keybind_remove") void __keybind_remove(const char* name, int keybind);
extern("keybind_get") int* __keybind_get(const char* name, int* count);
extern("keybind_get_entries") const char** __keybind_get_entries(int* count);
extern("keybind_down") bool __keybind_down(const char* name);
extern("keybind_pressed") bool __keybind_pressed(const char* name);
extern("keybind_released") bool __keybind_released(const char* name);
extern("keybind_mouse_down") bool __keybind_mouse_down(int button);
extern("keybind_mouse_pressed") bool __keybind_mouse_pressed(int button);
extern("keybind_mouse_released") bool __keybind_mouse_released(int button);
extern("keybind_mouse_x") float __keybind_mouse_x();
extern("keybind_mouse_y") float __keybind_mouse_y();
extern("keybind_update") void __keybind_update();
extern("keybind_get_mouse_scale") float __keybind_get_mouse_scale();
extern("keybind_set_mouse_scale") void __keybind_set_mouse_scale(float scale);

extern("storage_get_slot") StorageSlot* __storage_get_slot(uint32_t index);
extern("storage_add_slot") StorageSlot* __storage_add_slot();
extern("storage_remove_slot") void __storage_remove_slot(uint32_t index);
extern("storage_get") void* __storage_get(StorageSlot* storage, const char* name, size_t size);
extern("storage_num_slots") uint32_t __storage_num_slots();

extern("_get_asset") void* __get_asset(const char* name);

extern("get_micros") uint64_t __get_micros();
extern("watch_file") void __watch_file(const char* filename, FileWatchCallback callback);
extern("check_watched_files") void __check_watched_files();

extern("check_editor_mode") bool __editor_mode();

LevelRootNode* __curr_level_node;
Level __curr_level_loader;
struct {
    float progress, time;
    int direction;
    void(*func)();
} __curr_transition;

typedef struct {
    Node* curr_node;
    int ptr;
} NodeBuilder;

Engine* engine;
Graphics* gfx;
Assets* assets;
Input* input;
Storage* storage;

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
uint64_t get_millis(Engine* this) -> __get_micros() / 1000;
void watch_file(Engine* this, const char* filename, FileWatchCallback callback) -> __watch_file(filename, callback);
void check_watched_files(Engine* this) -> __check_watched_files();
bool editor_mode(Engine* this) -> __editor_mode();
bool create_transition(Engine* this, void(*func)(), float time, int direction) {
    if (__curr_transition.progress < 1) return false;
    __curr_transition.func = func;
    __curr_transition.time = time;
    __curr_transition.direction = direction;
    __curr_transition.progress = 0;
    return true;
}

void attach(Node* this, Node* child) -> __engine_attach_node(this, child);
void detach(Node* this) -> __engine_detach_node(this);
void delete(Node* this) -> __engine_delete_node(this);
Node* copy(Node* this) -> __engine_copy_node(this);
void set(TilemapNode* this, int x, int y, Tile tile) -> __engine_set_tile(this, x, y, tile);
uint8_t get(TilemapNode* this, int x, int y) -> __engine_get_tile(this, x, y);
<T> T* prop(EntityNode* this, const char* name) -> (T*)__engine_property(this, name);

void damage(EntityNode* this, EntityNode* source) {
    for (int i = 0; i < this.node.children_size; i++) {
        if (this.node.children[i] && (int)this.node.children[i].type == NodeType_EntityDamage) {
            void(*func)(EntityNode*, EntityNode*, TilemapNode*) = ((EntityDamageNode*)this.node.children[i]).func;
            func(this, source, this.node.parent);
        }
    }
}

void load(Engine* this, Level level) {
    if (__curr_level_node) __curr_level_node.node.delete();
    __curr_level_loader = level;
    __curr_level_node = level();
}

void reload(Engine* this) -> this.load(__curr_level_loader);
LevelRootNode* level(Engine* this) -> __curr_level_node;
TilesetNode* tileset(TilemapNode* this) -> __engine_get_tileset(this);

EntityNode* find(LevelRootNode* this, const char* name) -> __engine_find_entity(this, name);
EntityNode* find(TilemapNode* this, const char* name) -> __engine_find_entity_on_tilemap(this, name);

void update(LevelRootNode* this, float delta_time) -> __engine_update(this, delta_time);
void render(LevelRootNode* this, float width, float height) -> __engine_render(this, width, height);
void cleanup(Engine* this) -> __engine_cleanup();

Window* main(Graphics* this) -> NULL;
Window* open(Graphics* this, const char* title, int width, int height) -> __graphics_open(title, width, height);
void close(Window* this) -> __graphics_close(this);
void focus(Window* this) -> __graphics_focus(this);
void get_size(Window* this, int* width, int* height) -> __graphics_get_size(this, width, height);
void get_pos(Window* this, int* x, int* y) -> __graphics_get_pos(this, x, y);
void get_size(Graphics* this, int* x, int* y) -> __graphics_screen_size(x, y);
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

void stop(AudioInstance* this) -> __audio_stop(this);
void pause(AudioInstance* this) -> __audio_pause(this);
void resume(AudioInstance* this) -> __audio_resume(this);
void seek(AudioInstance* this, float sec) -> __audio_seek(this, sec);
void rate(AudioInstance* this, float factor) -> __audio_rate(this, factor);
void volume(AudioInstance* this, float volume) -> __audio_volume(this, volume);
bool is_paused(AudioInstance* this) -> __audio_is_paused(this);
bool is_playing(AudioInstance* this) -> __audio_is_playing(this);
bool is_done(AudioInstance* this) -> __audio_is_done(this);
float get_volume(AudioInstance* this) -> __audio_get_volume(this);
void play_oneshot(AudioSource* this) -> __audio_play_oneshot(this);
AudioInstance* create(AudioSource* this) -> __audio_play(this);

void add_entry(Input* this, const char* name) -> __keybind_add_entry(name);
void remove_entry(Input* this, const char* name) -> __keybind_remove_entry(name);
void add(Input* this, const char* name, int keybind) -> __keybind_add(name, keybind);
void remove(Input* this, const char* name, int keybind) -> __keybind_remove(name, keybind);
int* get(Input* this, const char* name, int* count) -> __keybind_get(name, count);
const char** get_entries(Input* this, int* count) -> __keybind_get_entries(count);
bool down(Input* this, const char* name) -> __keybind_down(name);
bool pressed(Input* this, const char* name) -> __keybind_pressed(name);
bool released(Input* this, const char* name) -> __keybind_released(name);
bool mouse_down(Input* this, int button) -> __keybind_mouse_down(button);
bool mouse_pressed(Input* this, int button) -> __keybind_mouse_pressed(button);
bool mouse_released(Input* this, int button) -> __keybind_mouse_released(button);
float mouse_x(Input* this) -> __keybind_mouse_x();
float mouse_y(Input* this) -> __keybind_mouse_y();
void update(Input* this) -> __keybind_update();
float get_mouse_scale(Input* this) -> __keybind_get_mouse_scale();
void set_mouse_scale(Input* this, float scale) -> __keybind_set_mouse_scale(scale);

<T> T* get(Assets* this, const char* name) -> __get_asset(name);

StorageSlot* __curr_storage;

<T> T* get(StorageSlot* this, const char* name) -> __storage_get(this, name, sizeof(T));
<T> T* get(Storage* this, const char* name) -> this.curr().get<T>(name);

int count(Storage* this) -> __storage_num_slots();
StorageSlot* add(Storage* this) -> __storage_add_slot();
StorageSlot* slot(Storage* this, uint32_t index) -> __storage_get_slot(index);
StorageSlot* curr(Storage* this) -> __curr_storage;
StorageSlot* load(Storage* this, uint32_t index) -> __curr_storage = this.slot(index);
StorageSlot* use(Storage* this, StorageSlot* slot) -> __curr_storage = slot;
uint32_t num_slots(Storage* this) -> __storage_num_slots();
void remove(StorageSlot* this) {
    if (__curr_storage == this) __curr_storage = nullptr;
    __storage_remove_slot(this);
}

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

NodeBuilder* tilemap(NodeBuilder* this, int width, int height, Tile(*oob_tile_provider)(TilemapNode* tilemap, int x, int y), Tile* tiles) {
    Tile* data = tiles ? malloc(width * height) : NULL;
    if (data) memcpy(data, tiles, width * height);
    TilemapNode* node = this.curr_node;
    node.start_x = node.start_y = 0;
    node.end_x = width;
    node.end_y = height;
    node.oob_tile_provider = oob_tile_provider;
    node.tiles = data;
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
