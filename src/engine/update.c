#include "engine/engine.h"

#include <stdlib.h>
#include <string.h>

TilesetNode* engine_get_tileset(TilemapNode* tilemap) {
    TilesetNode* tileset = NULL;
    for (int i = 0; i < tilemap->node.children_size && !tileset; i++) {
        if (!tilemap->node.children[i]) continue;
        if (tilemap->node.children[i]->type != NodeType_Tileset) continue;
        tileset = (TilesetNode*)tilemap->node.children[i];
    }
    return tileset;
}

static void engine_collision_event(Node* node, EntityNode* entity, TilemapNode* tilemap, TileNode* tile, int x, int y, Direction dir) {
    for (int i = 0; i < node->children_size; i++) {
        if (!node->children[i]) continue;
        if (node->children[i]->type != NodeType_Collision) continue;
        ((CollisionNode*)node->children[i])->func(entity, tilemap, tile, x, y, dir);
    }
}

typedef enum {
    Axis_X,
    Axis_Y,
} Axis;

static bool engine_rect_intersect(
    float x1a, float y1a, float x2a, float y2a,
    float x1b, float y1b, float x2b, float y2b
) {
    return x2a > x1b && x2b > x1a && y2a > y1b && y2b > y1a;
}

static void engine_resolve_collision(EntityNode* entity, TilemapNode* tilemap, TilesetNode* tileset, Axis axis) {
    float fx = entity->pos_x - entity->width / 2;
    float fy = entity->pos_y - entity->height;
    float tx = entity->pos_x + entity->width / 2;
    float ty = entity->pos_y;
    int min_x = floorf(fx) - 1;
    int min_y = floorf(fy) - 1;
    int max_x = ceilf(fx) + 1;
    int max_y = ceilf(ty) + 1;
    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            TileNode* tile = (TileNode*)tileset->node.children[*engine_tile(tilemap, x, y)];
            bool solid = tile->is_solid;
            if (!engine_rect_intersect(fx, fy, tx, ty, x, y, x + 1, y + 1)) continue;
            if (axis == Axis_X) {
                if (entity->vel_x == 0) (void)0;
                else if (entity->vel_x > 0) {
                    if (solid) {
                        entity->pos_x = x - entity->width / 2;
                        *(Direction*)engine_property(entity, "hor_collision") = Direction_Right;
                    }
                    engine_collision_event((Node*)tile,   entity, tilemap, tile, x, y, Direction_Left);
                    engine_collision_event((Node*)entity, entity, tilemap, tile, x, y, Direction_Right);
                }
                else if (entity->vel_x < 0) {
                    if (solid) {
                        entity->pos_x = x + 1 + entity->width / 2;
                        *(Direction*)engine_property(entity, "hor_collision") = Direction_Left;
                    }
                    engine_collision_event((Node*)tile,   entity, tilemap, tile, x, y, Direction_Right);
                    engine_collision_event((Node*)entity, entity, tilemap, tile, x, y, Direction_Left);
                }
                if (solid) entity->vel_x = 0;
            }
            if (axis == Axis_Y) {
                if (entity->vel_y == 0) (void)0;
                else if (entity->vel_y > 0) {
                    if (solid) {
                        entity->pos_y = y;
                        *(Direction*)engine_property(entity, "ver_collision") = Direction_Down;
                    }
                    engine_collision_event((Node*)tile,   entity, tilemap, tile, x, y, Direction_Up);
                    engine_collision_event((Node*)entity, entity, tilemap, tile, x, y, Direction_Down);
                }
                else if (entity->vel_y < 0) {
                    if (solid) {
                        entity->pos_y = y + 1 + entity->height;
                        *(Direction*)engine_property(entity, "ver_collision") = Direction_Up;
                    }
                    engine_collision_event((Node*)tile,   entity, tilemap, tile, x, y, Direction_Down);
                    engine_collision_event((Node*)entity, entity, tilemap, tile, x, y, Direction_Up);
                }
                if (solid) {
                    *(bool*)engine_property(entity, "touching_ground") = entity->vel_y >= 0;
                    entity->vel_y = 0;
                }
            }
        }
    }
}

static void engine_update_entity(EntityNode* entity, TilemapNode* tilemap, TilesetNode* tileset, int index, float delta_time) {
    for (int i = 0; i < entity->node.children_size; i++) {
        if (!entity->node.children[i]) continue;
        if (entity->node.children[i]->type != NodeType_EntityUpdate) continue;
        ((EntityUpdateNode*)entity->node.children[i])->func(entity, tilemap, delta_time);
        if (!tilemap->node.children[index]) return; // entity deleted
    }
    *(bool*)engine_property(entity, "touching_ground") = false;
    *(Direction*)engine_property(entity, "hor_collision") = *(Direction*)engine_property(entity, "ver_collision") = Direction_None;
    entity->pos_y += entity->vel_y * delta_time;
    engine_resolve_collision(entity, tilemap, tileset, Axis_Y);
    entity->pos_x += entity->vel_x * delta_time;
    engine_resolve_collision(entity, tilemap, tileset, Axis_X);
    if (*(Direction*)engine_property(entity, "hor_collision") != Direction_None) *(Direction*)engine_property(entity, "last_hor_collision") = *(Direction*)engine_property(entity, "hor_collision");
    if (*(Direction*)engine_property(entity, "ver_collision") != Direction_None) *(Direction*)engine_property(entity, "last_ver_collision") = *(Direction*)engine_property(entity, "ver_collision");
    *(float*)engine_property(entity, "delta_time") += delta_time;
}

static void engine_update_tilemap(TilemapNode* tilemap, TilesetNode* tileset, float delta_time) {
    for (int i = 0; i < tilemap->node.children_size; i++) {
        if (!tilemap->node.children[i]) continue;
        if (tilemap->node.children[i]->type != NodeType_Entity) continue;
        EntityNode* entity = (EntityNode*)tilemap->node.children[i];
        engine_update_entity(entity, tilemap, tileset, i, delta_time);
    }
}

void engine_update(LevelRootNode* node, float delta_time) {
    for (int i = 0; i < node->node.children_size; i++) {
        if (!node->node.children[i]) continue;
        if (node->node.children[i]->type != NodeType_Tilemap) continue;
        TilemapNode* tilemap = (TilemapNode*)node->node.children[i];
        engine_update_tilemap(tilemap, engine_get_tileset(tilemap), delta_time);
    }
}

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
    }
    node->data.entries[node->data.count++].key = (char*)name;
    qsort(node->data.entries, node->data.count, sizeof(*node->data.entries), compare_string);
    return &((typeof(*node->data.entries)*)bsearch(&name, node->data.entries, node->data.count, sizeof(*node->data.entries), compare_string))->value;
}
