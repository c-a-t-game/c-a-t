#include "engine.h"

#include <stdlib.h>
#include <string.h>

static int compare_string(const void* a, const void* b) {
    return strcmp(*(char**)a, *(char**)b);
}

uint8_t* engine_tile(TilemapNode* node, int x, int y) {
    if (x < 0 || y < 0 || x >= node->width || y >= node->height) return &node->oob_tile;
    return &node->tiles[y * node->width + x];
}

void* engine_property(EntityNode* node, const char* name) {
    if (node->data.entries) {
        void* ptr = bsearch(&name, node->data.entries, node->data.count, sizeof(*node->data.entries), compare_string);
        if (ptr) return &((typeof(*node->data.entries)*)ptr)->value;
    }
    if (node->data.count == node->data.capacity) {
        if (node->data.capacity == 0) node->data.capacity = 4;
        else node->data.capacity *= 2;
        node->data.entries = realloc(node->data.entries, node->data.capacity * sizeof(*node->data.entries));
        memset(node->data.entries + node->data.count, 0, (node->data.capacity - node->data.count) * sizeof(*node->data.entries));
    }
    node->data.entries[node->data.count++].key = (char*)name;
    qsort(node->data.entries, node->data.count, sizeof(*node->data.entries), compare_string);
    return &((typeof(*node->data.entries)*)bsearch(&name, node->data.entries, node->data.count, sizeof(*node->data.entries), compare_string))->value;
}

EntityNode* engine_find_entity(LevelRootNode* node, const char* name) {
    for (int i = 0; i < node->node.children_size; i++) {
        if (node->node.children[i]->type != NodeType_Tilemap) continue;
        EntityNode* node = engine_find_entity_on_tilemap((TilemapNode*)node->node.children[i], name);
        if (node) return node;
    }
    return NULL;
}

EntityNode* engine_find_entity_on_tilemap(TilemapNode* node, const char* name) {
    for (int i = 0; i < node->node.children_size; i++) {
        if (node->node.children[i]->type != NodeType_Entity) continue;
        EntityNode* entity = (EntityNode*)node->node.children[i];
        if (entity->name && strcmp(entity->name, name) == 0) return entity;
    }
    return NULL;
}
