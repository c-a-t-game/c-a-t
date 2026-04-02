#depends "scripts/entities/heart.c"

Node* entity_coin(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(0) // vel_x
    .prop<float>(0) // vel_y
    .prop<float>(0) // width
    .prop<float>(0) // height
    .prop<const char*>("entity_coin") // func
    .event<EntityUpdateNode>(lambda(EntityNode* entity, TilemapNode* tilemap, float delta_time): void {
        *entity.prop<Texture*>("texture") = assets.get<Texture>("images/entities/coin.png");
        *entity.prop<char*>("target_storage") = "num_coins";
    })
    .event<EntityUpdateNode>(entity_heart_update)
    .event<EntityTextureNode>(entity_heart_texture)
    .event<EntityCollisionNode>(entity_heart_collision)
.build();

Node* entity_purple_coin(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(0) // vel_x
    .prop<float>(0) // vel_y
    .prop<float>(0) // width
    .prop<float>(0) // height
    .prop<const char*>("entity_purple_coin") // func
    .event<EntityUpdateNode>(lambda(EntityNode* entity, TilemapNode* tilemap, float delta_time): void {
        *entity.prop<Texture*>("texture") = assets.get<Texture>("images/entities/purple_coin.png");
    })
    .event<EntityUpdateNode>(entity_heart_update)
    .event<EntityTextureNode>(entity_heart_texture)
    .event<EntityCollisionNode>(entity_heart_collision)
.build();
