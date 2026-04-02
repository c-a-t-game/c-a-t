#depends "scripts/engine.c"
#depends "scripts/editor.c"

Node* entity_level_end(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(0) // vel_x
    .prop<float>(0) // vel_y
    .prop<float>(1) // width
    .prop<float>(1) // height
    .prop<const char*>("entity_level_end") // func
    .prop<const char*>("end") // name
    .event<EntityUpdateNode>(lambda entity_level_end_update(EntityNode* entity, TilemapNode* tilemap): void {
        *entity.prop<int>("num_coins") = 0;
        for (int y = tilemap.start_y; y < tilemap.end_y; y++) {
            for (int x = tilemap.start_x; x < tilemap.end_x; x++) {
                if (tilemap.get(x, y) == 9)
                    (*entity.prop<int>("num_coins"))++;
            }
        }
        for (int i = 0; i < tilemap.node.children_size; i++) {
            if (tilemap.node.children[i] && (int)tilemap.node.children[i].type == NodeType_Entity) {
                EntityNode* coin = tilemap.node.children[i];
                if (coin.func && strcmp(coin.func, "entity_purple_coin") == 0)
                    (*entity.prop<int>("num_coins"))++;
            }
        }
        if (*entity.prop<int>("max_coins") < *entity.prop<int>("num_coins"))
            *entity.prop<int>("max_coins") = *entity.prop<int>("num_coins");
    })
    .event<EntityCollisionNode>(lambda entity_level_end_collision(EntityNode* collidee, EntityNode* collider, TilemapNode* tilemap): void {
        if (editor_is_editing() || editor_toggle_play_mode) return;
        if (!collider.is("player")) return;
        if (*collidee.prop<int>("num_coins") != 0) return;
        *collider.prop<bool>("napping") = true;
        collider.pos_x = collidee.pos_x;
        collider.pos_y = collidee.pos_y;
    })
    .event<EntityTextureNode>(lambda entity_nap_spot_texture(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h): Texture* {
        *w = *srcw = 32;
        if (entity.is("start")) *srcx = 0;
        else *srcx = *entity.prop<int>("num_coins") == 0 ? 0 : 32;
        return assets.get<Texture>("images/entities/nap_spot.png");
    })
.build();

Node* entity_level_start(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(0) // vel_x
    .prop<float>(0) // vel_y
    .prop<float>(1) // width
    .prop<float>(1) // height
    .prop<const char*>("entity_level_start") // func
    .prop<const char*>("start") // name
    .event<EntityTextureNode>(entity_nap_spot_texture)
.build();
