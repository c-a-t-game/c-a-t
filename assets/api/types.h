#ifndef TYPES_H
#define TYPES_H

#include "stdint.h"

#ifndef __JITC__
#define __ID__(a, b) a##b
#endif

typedef enum {
#define NODE(type, ...) __ID__(NodeType_, type),
#include "api/nodes.h"
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
#include "api/nodes.h"
#undef NODE
#undef _

typedef struct Shader Shader;
typedef struct Window Window;
typedef struct Buffer Buffer;

#endif
