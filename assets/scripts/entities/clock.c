#depends "scripts/entities/heart.c"

Node* entity_clock(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(0) // vel_x
    .prop<float>(0) // vel_y
    .prop<float>(0) // width
    .prop<float>(0) // height
    .prop<const char*>("entity_coin") // func
    .event<EntityUpdateNode>(lambda(EntityNode* entity, TilemapNode* tilemap, float delta_time): void {
        *entity.prop<Texture*>("texture") = assets.get<Texture>("images/entities/clock.png");
    })
    .event<EntityUpdateNode>(entity_heart_update)
    .event<EntityTextureNode>(entity_heart_texture)
    .event<EntityCollisionNode>(entity_heart_collision)
    .event<EntityCollisionNode>(lambda entity_clock_collision(EntityNode* collidee, EntityNode* collider, TilemapNode* node): void {
        if (editor_is_editing()) return;
        if (!collider.is("player")) return;
        sound_get_heart().play_oneshot();
        *collider.prop<float>("time_until_death") = 600;
    })
.build();
