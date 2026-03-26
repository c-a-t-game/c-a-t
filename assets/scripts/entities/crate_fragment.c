#depends "scripts/engine.c"
#depends "scripts/random.c"

Node* entity_crate_fragment(int x, int y) -> engine.open<EntityNode>()
    .prop<float>(x + 0.5)  // pos_x
    .prop<float>(y + 0.75) // pos_y
    .prop<float>(frng(-0.2,  0.2)) // vel_x
    .prop<float>(frng(-0.3, -0.1)) // vel_y
    .prop<float>(0.5) // width
    .prop<float>(0.5) // height
    .event<EntityUpdateNode>(lambda entity_crate_fragment_update(EntityNode* entity, TilemapNode* tilemap, float delta_time): void {
        float* timer = entity.prop<float>("texture_timer");
        if (!*entity.prop<bool>("touching_ground")) *timer += delta_time;
        else entity.vel_x = 0;
        if (*timer > 20) *timer -= 20; // for some reason changing this to a while cooks the entire compiler
        *entity.prop<int>("flip_state") = *timer / 5;
        entity.vel_y += 0.03;
    })
    .event<EntityTextureNode>(lambda entity_crate_fragment_texture(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h): Texture* {
        *srcx = *srcy = 0;
        *srcw = *srch = *w = *h = 8;
        if (*entity.prop<int>("flip_state") & (1 << 0)) *w *= -1;
        if (*entity.prop<int>("flip_state") & (1 << 1)) *h *= -1;
        return assets.get<Texture>("images/entities/crate_fragment.png");
    })
.build();
