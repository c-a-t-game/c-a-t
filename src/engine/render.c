#include "engine.h"

#include "io/platform.h"

#include <stdlib.h>

extern TilesetNode* engine_get_tileset(TilemapNode* tilemap);

static void engine_get_tilemap_offsets(TilemapNode* tilemap, TilesetNode* tileset, float cam_x, float cam_y, float* offset_x, float* offset_y) {
    *offset_x = cam_x * tilemap->scroll_speed_x / tilemap->scale_x / tileset->tile_width  - tilemap->scroll_offset_x;
    *offset_y = cam_y * tilemap->scroll_speed_y / tilemap->scale_y / tileset->tile_height - tilemap->scroll_offset_y;
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
        ((entity->pos_x - offset_x) * tileset->tile_width  - w / 2) * tilemap->scale_x,
        ((entity->pos_y - offset_y) * tileset->tile_height - h)     * tilemap->scale_y,
        w * tilemap->scale_x, h * tilemap->scale_y, sx, sy, sw, sh, GRAY(255)
    );
}

static void engine_render_tile(TilemapNode* tilemap, TilesetNode* tileset, float x, float y, float offset_x, float offset_y) {
    TileNode* tile = (TileNode*)tileset->node.children[*engine_tile(tilemap, x, y)];
    if (tile == NULL || tile->node.type != NodeType_Tile) return;
    int index = -1;
    for (int i = 0; i < tile->node.children_size && index == -1; i++) {
        if (!tile->node.children[i]) continue;
        if (tile->node.children[i]->type == NodeType_TileTexture)
            index = ((TileTextureNode*)tile->node.children[i])->func(tilemap, tile, x, y);
    }
    if (index == -1) return;
    graphics_draw(NULL, tileset->tileset,
        (x - offset_x) * tilemap->scale_x * tileset->tile_width,
        (y - offset_y) * tilemap->scale_y * tileset->tile_height,
        tileset->tile_width * tilemap->scale_x, tileset->tile_height * tilemap->scale_y,
        (int)(index % tileset->tiles_per_row) * tileset->tile_width,
        (int)(index / tileset->tiles_per_row) * tileset->tile_height,
        tileset->tile_width, tileset->tile_height,
        GRAY(255)
    );
}

static void engine_render_tilemap(TilemapNode* tilemap, float width, float height, float cam_x, float cam_y) {
    TilesetNode* tileset = engine_get_tileset(tilemap);
    float offset_x, offset_y;
    engine_get_tilemap_offsets(tilemap, tileset, cam_x, cam_y, &offset_x, &offset_y);
    int min_x = floorf(offset_x);
    int min_y = floorf(offset_y);
    int max_x = ceilf((offset_x + width  / tilemap->scale_x / tileset->tile_width));
    int max_y = ceilf((offset_y + height / tilemap->scale_y / tileset->tile_height));
    for (int x = min_x; x <= max_x; x++) {
        for (int y = min_y; y <= max_y; y++) {
            engine_render_tile(tilemap, tileset, x, y, offset_x, offset_y);
        }
    }
    for (int i = 0; i < tilemap->node.children_size; i++) {
        if (!tilemap->node.children[i]) continue;
        if (tilemap->node.children[i]->type == NodeType_Entity)
            engine_render_entity((EntityNode*)tilemap->node.children[i], tileset, offset_x, offset_y);
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
