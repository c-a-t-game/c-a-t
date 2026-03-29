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
    .event<EntityCollisionNode>(lambda entity_level_end_collision(EntityNode* collidee, EntityNode* collider, TilemapNode* tilemap): void {
        if (editor_is_editing() || editor_toggle_play_mode) return;
        if (!collider.is("player")) return;
        *collider.prop<bool>("napping") = true;
        collider.pos_x = collidee.pos_x;
        collider.pos_y = collidee.pos_y;
    })
    .event<EntityTextureNode>(lambda entity_nap_spot_texture(): Texture* -> assets.get<Texture>("images/entities/nap_spot.png"))
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
