#include "engine.h"

#include "io/assets.h"

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdalign.h>

static void _(void* ptr, int size, uint8_t** bytes) {}

static void raw(void* ptr, int size, uint8_t** bytes) {
    memcpy(ptr, *bytes, size);
    *bytes += size;
}

static void polygon(void* ptr, int size, uint8_t** bytes) {
    gpc_polygon* p = ptr;
    p->num_contours = *(uint32_t*)*bytes; *bytes += 4;
    p->hole = malloc(p->num_contours * sizeof(int));
    p->contour = malloc(p->num_contours * sizeof(gpc_vertex_list));
    for (int i = 0; i < p->num_contours; i++) {
        gpc_vertex_list* vl = &p->contour[i];
        p->hole[i] = *(uint8_t*)*bytes; *bytes += 1;
        vl->num_vertices = *(uint32_t*)*bytes; *bytes += 4;
        vl->vertex = malloc(vl->num_vertices * sizeof(gpc_vertex));
        for (int j = 0; j < vl->num_vertices; j++) {
            gpc_vertex* v = &vl->vertex[j];
            v->x = *(float*)*bytes; *bytes += 4;
            v->y = *(float*)*bytes; *bytes += 4;
        }
    }
}

static void asset(void* ptr, int size, uint8_t** bytes) {
    char* str = (char*)*bytes; *bytes += strlen(str) + 1;
    *(void**)ptr = get_asset(void, str);
}

typedef struct {
    int size, align;
    void(*parser)(void* ptr, int size, uint8_t** bytes);
} FieldInfo;

static struct {
    int size;
    int num_fields;
    FieldInfo* fields;
} node_info[] = {
#define _(type, name, parser) { sizeof(type), alignof(type), parser },
#define NODE(type, ...) { \
    sizeof(type##Node), \
    sizeof((FieldInfo[]){ __VA_ARGS__ }) / sizeof(FieldInfo) + 1, \
    (FieldInfo[]){ _(Node, node, _) __VA_ARGS__ } \
},
#include "nodes.h"
#undef NODE
#undef _
};

Node* parse_node(uint8_t** bytes) {
    uint8_t type = **bytes;
    *bytes += 1;
    if (type == 0) {
        char* name = (char*)*bytes;
        return engine_deep_copy(get_asset(Node, name));
    }
    type--;
    Node* node = calloc(node_info[type].size, 1);
    int pos = 0;
    for (int i = 0; i < node_info[type].num_fields; i++) {
        FieldInfo* field = &node_info[type].fields[i];
        if (pos % field->align != 0) pos += field->align - pos % field->align;
        field->parser((char*)node + pos, field->size, bytes);
        pos += field->size;
    }
    node->type = type;
    node->size = node_info[type].size;
    node->children_size = *(uint32_t*)*bytes;
    *bytes += 4;
    for (int i = 0; i < node->children_size; i++) {
        engine_attach_node(node, parse_node(bytes));
    }
    return node;
}

void* loader_lvl(uint8_t* bytes, int size) {
    return parse_node(&bytes);
}
