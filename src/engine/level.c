#include "engine.h"

#include <stdlib.h>

static void engine_resolve_collision() {
    
}

static void engine_update_entity(EntityNode* entity, TilemapNode* tilemap) {
    for (int i = 0; i < entity->node.children_size; i++) {
        if (!entity->node.children[i]) continue;
        if (entity->node.children[i]->type != NodeType_EntityUpdate) continue;
        ((EntityUpdateNode*)entity->node.children[i])->func(entity, tilemap);
    }
    entity->pos_y += entity->vel_y;
    engine_resolve_collision();
    entity->pos_x += entity->vel_x;
    engine_resolve_collision();
}

static void engine_update_tilemap(TilemapNode* tilemap) {
    for (int i = 0; i < tilemap->node.children_size; i++) {
        if (!tilemap->node.children[i]) continue;
        if (tilemap->node.children[i]->type != NodeType_Entity) continue;
        EntityNode* entity = (EntityNode*)tilemap->node.children[i];
        engine_update_entity(entity, tilemap);
    }
}

void engine_update(LevelRootNode* node) {
    for (int i = 0; i < node->node.children_size; i++) {
        if (!node->node.children[i]) continue;
        if (node->node.children[i]->type != NodeType_Tilemap) continue;
        TilemapNode* tilemap = (TilemapNode*)node->node.children[i];
        engine_update_tilemap(tilemap);
    }
}

static TilesetNode* engine_get_tileset(TilemapNode* tilemap) {
    TilesetNode* tileset = NULL;
    for (int i = 0; i < tilemap->node.children_size && !tileset; i++) {
        if (!tilemap->node.children[i]) continue;
        if (tilemap->node.children[i]->type != NodeType_Tileset) continue;
        tileset = (TilesetNode*)tilemap->node.children[i];
    }
    return tileset;
}

static void engine_get_tilemap_offsets(TilemapNode* tilemap, float cam_x, float cam_y, float* offset_x, float* offset_y) {
    *offset_x = cam_x * tilemap->scroll_speed_x / tilemap->scale_x + tilemap->scroll_offset_x;
    *offset_y = cam_y * tilemap->scroll_speed_y / tilemap->scale_y + tilemap->scroll_offset_y;
}

static void engine_render_entity(EntityNode* entity, TilesetNode* tileset, float offset_x, float offset_y) {
    TilemapNode* tilemap = (TilemapNode*)entity->node.parent;
    Texture* tex = NULL;
    float sx = NAN, sy = NAN, sw = NAN, sh = NAN, w = NAN, h = NAN;
    for (int i = 0; i < entity->node.children_size && !tex; i++) {
        if (!entity->node.children[i]) continue;
        if (entity->node.children[i]->type != NodeType_EntityTexture) continue;
        tex = ((EntityTextureNode*)entity->node.children[i])->func(entity, tilemap, &sx, &sy, &sw, &sh, &w, &h);
    }
    if (!tex) return;
    if (isnan(sx)) sx = 0;
    if (isnan(sy)) sy = 0;
    if (isnan(sw)) sw = tex->width;
    if (isnan(sh)) sh = tex->height;
    if (isnan(w)) w = tex->width;
    if (isnan(h)) h = tex->height;
    graphics_draw(NULL, tex,
        (entity->pos_x * tileset->tile_width  - w / 2) * tilemap->scale_x,
        (entity->pos_y * tileset->tile_height - h)     * tilemap->scale_y,
        w * tilemap->scale_x, h * tilemap->scale_y, sx, sy, sw, sh, GRAY(255)
    );
}

static void engine_render_chunk(TileChunkNode* chunk, TilesetNode* tileset, float offset_x, float offset_y) {
    TilemapNode* tilemap = (TilemapNode*)chunk->node.parent;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            TileNode* tile = (TileNode*)tileset->node.children[chunk->tile[x][y]];
            if (tile == NULL || tile->node.type != NodeType_Tile) continue;
            int index = -1;
            for (int i = 0; i < tile->node.children_size && index == -1; i++) {
                if (!tile->node.children[i]) continue;
                if (tile->node.children[i]->type != NodeType_TileTexture) continue;
                index = ((TileTextureNode*)tile->node.children[i])->func(tilemap, tile, chunk->x * CHUNK_SIZE + x, chunk->y * CHUNK_SIZE + y);
            }
            if (index == -1) continue;
            graphics_draw(NULL, tileset->tileset,
                (x + chunk->x * CHUNK_SIZE - offset_x) * tilemap->scale_x * tileset->tile_width,
                (y + chunk->y * CHUNK_SIZE - offset_y) * tilemap->scale_y * tileset->tile_height,
                tileset->tile_width * tilemap->scale_x, tileset->tile_height * tilemap->scale_y,
                (int)(index % tileset->tiles_per_row) * tileset->tile_width,
                (int)(index / tileset->tiles_per_row) * tileset->tile_height,
                tileset->tile_width, tileset->tile_height,
                GRAY(255)
            );
        }
    }
}

static void engine_render_tilemap(TilemapNode* tilemap, float width, float height, float cam_x, float cam_y) {
    TilesetNode* tileset = engine_get_tileset(tilemap);
    float offset_x, offset_y;
    engine_get_tilemap_offsets(tilemap, cam_x, cam_y, &offset_x, &offset_y);
    int min_x = floorf(offset_x / CHUNK_SIZE);
    int min_y = floorf(offset_y / CHUNK_SIZE);
    int max_x = ceilf((offset_x + width  / tilemap->scale_x / tileset->tile_width)  / CHUNK_SIZE);
    int max_y = ceilf((offset_x + height / tilemap->scale_y / tileset->tile_height) / CHUNK_SIZE);
    for (int i = 0; i < tilemap->node.children_size; i++) {
        if (!tilemap->node.children[i]) continue;
        if (tilemap->node.children[i]->type == NodeType_Entity)
            engine_render_entity((EntityNode*)tilemap->node.children[i], tileset, offset_x, offset_y);
        if (tilemap->node.children[i]->type == NodeType_TileChunk) {
            TileChunkNode* chunk = (TileChunkNode*)tilemap->node.children[i];
            if (chunk->x < min_x || chunk->y < min_y || chunk->x > max_x || chunk->y > max_y) continue;
            engine_render_chunk(chunk, tileset, offset_x, offset_y);
        }
    }
}

void engine_render(LevelRootNode* level, float width, float height) {
    for (int i = 0; i < level->node.children_size; i++) {
        if (!level->node.children[i]) continue;
        if (level->node.children[i]->type != NodeType_Tilemap) continue;
        TilemapNode* tilemap = (TilemapNode*)level->node.children[i];
        engine_render_tilemap(tilemap, width, height, level->cam_x, level->cam_y);
    }
}