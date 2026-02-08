#include "engine.h"

Node* entity_player(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .event<EntityUpdateNode>(lambda(EntityNode* node, TilemapNode* tilemap, float delta_time): void {
        if (input.down("up")) node.pos_y -= 1/8.f;
        if (input.down("down")) node.pos_y += 1/8.f;
        if (input.down("left")) node.pos_x -= 1/8.f;
        if (input.down("right")) node.pos_x += 1/8.f;
        ((LevelRootNode*)tilemap.node.parent).cam_x = node.pos_x * 16 - 192;
        ((LevelRootNode*)tilemap.node.parent).cam_y = node.pos_y * 16 - 128;
    })
    .event<EntityTextureNode>(lambda(EntityNode* node, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h): Texture* {
        return assets.get<Texture>("trolley.png");
    })
.build();