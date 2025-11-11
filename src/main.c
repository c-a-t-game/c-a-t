#include <stdio.h>
#include <stdlib.h>

#include "io/assets.h"
#include "io/graphics.h"
#include "io/input.h"
#include "io/audio.h"

#include "engine/engine.h"

int tile_texture(TilemapNode* tilemap, TileNode* tile, int x, int y) {
    return 0;
}

void player_update(EntityNode* node, TilemapNode* tilemap) {
    if (keybind_down("up")) node->pos_y -= 1/8.f;
    if (keybind_down("down")) node->pos_y += 1/8.f;
    if (keybind_down("left")) node->pos_x -= 1/8.f;
    if (keybind_down("right")) node->pos_x += 1/8.f;
    ((LevelRootNode*)tilemap->node.parent)->cam_x = node->pos_x * 16 - 192;
    ((LevelRootNode*)tilemap->node.parent)->cam_y = node->pos_y * 16 - 128;
}

Texture* player_texture(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h) {
    return get_asset(Texture, "trol.png");
}

int main() {
    load_assets();
    audio_init();
    keybind_add("up", 26);
    keybind_add("down", 22);
    keybind_add("left", 4);
    keybind_add("right", 7);
    Window* window = graphics_open(":3", 1152, 768);
    graphics_set_active(window);

    LevelRootNode* level = engine_new_node(LevelRoot);
    TilemapNode* bg = engine_new_node(Tilemap);
    bg->scale_x = 1;
    bg->scale_y = 1;
    bg->scroll_offset_x = -0.125;
    bg->scroll_offset_y = -0.125;
    bg->scroll_speed_x = 0.5f;
    bg->scroll_speed_y = 0.5f;
    engine_set_tile(bg, 0, 0, 1);
    TilesetNode* bg_tileset = engine_new_node(Tileset);
    bg_tileset->tileset = get_asset(Texture, "bg.png");
    bg_tileset->tile_width = 576;
    bg_tileset->tile_height = 384;
    bg_tileset->tiles_per_row = 1;
    TileNode* bg_air = engine_new_node(Tile);
    engine_attach_node(&bg_tileset->node, &bg_air->node);
    TileNode* bg_tile = engine_new_node(Tile);
    TileTextureNode* bg_tex = engine_new_node(TileTexture);
    bg_tex->func = tile_texture;
    engine_attach_node(&bg_tile->node, &bg_tex->node);
    engine_attach_node(&bg_tileset->node, &bg_tile->node);
    engine_attach_node(&bg->node, &bg_tileset->node);
    engine_attach_node(&level->node, &bg->node);
    TilemapNode* tilemap = engine_new_node(Tilemap);
    tilemap->scale_x = 1;
    tilemap->scale_y = 1;
    tilemap->scroll_offset_x = 0;
    tilemap->scroll_offset_y = 0;
    tilemap->scroll_speed_x = 1;
    tilemap->scroll_speed_y = 1;
    engine_set_tile(tilemap, 2, 1, 1);
    engine_set_tile(tilemap, 4, 1, 1);
    engine_set_tile(tilemap, 2, 2, 1);
    engine_set_tile(tilemap, 4, 2, 1);
    engine_set_tile(tilemap, 1, 4, 1);
    engine_set_tile(tilemap, 3, 4, 1);
    engine_set_tile(tilemap, 5, 4, 1);
    engine_set_tile(tilemap, 2, 5, 1);
    engine_set_tile(tilemap, 4, 5, 1);
    TilesetNode* tileset = engine_new_node(Tileset);
    tileset->tileset = get_asset(Texture, "trol.png");
    tileset->tile_width = 16;
    tileset->tile_height = 16;
    tileset->tiles_per_row = 16;
    TileNode* air = engine_new_node(Tile);
    engine_attach_node(&tileset->node, &air->node);
    TileNode* ground = engine_new_node(Tile);
    TileTextureNode* ground_tex = engine_new_node(TileTexture);
    ground_tex->func = tile_texture;
    engine_attach_node(&ground->node, &ground_tex->node);
    engine_attach_node(&tileset->node, &ground->node);
    engine_attach_node(&tilemap->node, &tileset->node);
    EntityNode* player = engine_new_node(Entity);
    player->pos_x = 1;
    player->pos_y = 2;
    EntityUpdateNode* entity_update = engine_new_node(EntityUpdate);
    entity_update->func = player_update;
    engine_attach_node(&player->node, &entity_update->node);
    EntityTextureNode* entity_texture = engine_new_node(EntityTexture);
    entity_texture->func = player_texture;
    engine_attach_node(&player->node, &entity_texture->node);
    engine_attach_node(&tilemap->node, &player->node);
    engine_attach_node(&level->node, &tilemap->node);

    while (!graphics_should_close()) {
        graphics_start_frame(NULL);
        Buffer* buffer = graphics_new_buffer(NULL, 384, 256);
        graphics_set_buffer(NULL, buffer);

        keybind_update();
        engine_update(level);
        engine_render(level, 1152, 768);

        graphics_set_buffer(NULL, NULL);
        graphics_blit(NULL, buffer, 0, 0, 1152, 768, 0, 0, 384, 256, GRAY(255));
        graphics_end_frame(NULL);
        graphics_destroy_buffer(buffer);
    }
    graphics_close(window);
    return 0;
}
