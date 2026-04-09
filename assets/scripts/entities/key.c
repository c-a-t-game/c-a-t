#depends "scripts/engine.c"
#depends "scripts/editor.c"

Texture* entity_key_texture(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h) {
    Texture* tex = *entity.prop<Texture*>("texture");
    *srch = *h = tex.height;
    *srcw = *w = tex.width / 2;
    *srcx = engine.get_millis() / 250 % 2 * tex.width / 2;
    *srcy = 0;
    return tex;
}

Node* entity_key_a(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(0) // vel_x
    .prop<float>(0) // vel_y
    .prop<float>(0) // width
    .prop<float>(0) // height
    .event<EntityTextureNode>(entity_key_texture)
    .event<EntityUpdateNode>(lambda(EntityNode* entity): void ->
        *entity.prop<Texture*>("texture") = assets.get<Texture>("images/entities/key_a.png")
    )
.build();

Node* entity_key_d(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(0) // vel_x
    .prop<float>(0) // vel_y
    .prop<float>(0) // width
    .prop<float>(0) // height
    .event<EntityTextureNode>(entity_key_texture)
    .event<EntityUpdateNode>(lambda(EntityNode* entity): void ->
        *entity.prop<Texture*>("texture") = assets.get<Texture>("images/entities/key_d.png")
    )
.build();

Node* entity_key_l(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(0) // vel_x
    .prop<float>(0) // vel_y
    .prop<float>(0) // width
    .prop<float>(0) // height
    .event<EntityTextureNode>(entity_key_texture)
    .event<EntityUpdateNode>(lambda(EntityNode* entity): void ->
        *entity.prop<Texture*>("texture") = assets.get<Texture>("images/entities/key_l.png")
    )
.build();

Node* entity_key_space(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(0) // vel_x
    .prop<float>(0) // vel_y
    .prop<float>(0) // width
    .prop<float>(0) // height
    .event<EntityTextureNode>(entity_key_texture)
    .event<EntityUpdateNode>(lambda(EntityNode* entity): void ->
        *entity.prop<Texture*>("texture") = assets.get<Texture>("images/entities/key_spc.png")
    )
.build();
