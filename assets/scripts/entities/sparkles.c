#depends "scripts/engine.c"

Node* entity_sparkles(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .event<EntityUpdateNode>(lambda entity_sparkles_update(EntityNode* entity, TilemapNode* tilemap, float delta_time): void {
        *entity.prop<float>("timer") += delta_time;
        if (*entity.prop<float>("timer") >= 32) entity.node.delete();
    })
    .event<EntityTextureNode>(lambda entity_sparkles_texture(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h): Texture* {
        int index = *entity.prop<float>("timer") / 8;
        *srcx = index * 16;
        *srcy = 0;
        *srcw = *srch = *w = *h = 16;
        return assets.get<Texture>("images/entities/sparkles.png");
    })
.build();
