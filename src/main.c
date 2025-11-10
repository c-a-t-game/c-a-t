#include <stdio.h>
#include <stdlib.h>

#include "io/assets.h"
#include "io/graphics.h"
#include "io/input.h"

#include "engine/engine.h"

int ground_texture(TilemapNode* tilemap, TileNode* tile, int x, int y) {
    return 0;
}

void player_update(EntityNode* node, TilemapNode* tilemap) {
    if (keybind_down("up")) node->pos_y -= 1/8.f;
    if (keybind_down("down")) node->pos_y += 1/8.f;
    if (keybind_down("left")) node->pos_x -= 1/8.f;
    if (keybind_down("right")) node->pos_x += 1/8.f;
}

Texture* player_texture(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h) {
    return get_asset(Texture, "trol.png");
}

int main() {
    load_assets();
    Window* window = graphics_open(":3", 1152, 768);
    graphics_set_active(window);
    
    LevelRootNode* level = engine_new_node(LevelRoot);
    TilemapNode* tilemap = engine_new_node(Tilemap);
    tilemap->scale_x = 3;
    tilemap->scale_y = 3;
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
    ground_tex->func = ground_texture;
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
    
    keybind_add("up", 26);
    keybind_add("down", 22);
    keybind_add("left", 4);
    keybind_add("right", 7);
    
    while (!graphics_should_close()) {
        graphics_start_frame(NULL);
        /*Buffer* buffer = graphics_new_buffer(NULL, 384, 256);
        graphics_set_buffer(NULL, buffer);*/
        
        keybind_update();
        engine_update(level);
        engine_render(level, 1152, 768);
        
        /*graphics_set_buffer(NULL, NULL);
        graphics_blit(NULL, buffer, 0, 0, 1152, 768, 0, 0, 384, 256, GRAY(255));*/
        graphics_end_frame(NULL);
        //graphics_destroy_buffer(buffer);
    }
    graphics_close(window);
    return 0;
}
