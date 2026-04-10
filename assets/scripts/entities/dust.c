#depends "scripts/engine.c"

Node* entity_dust(float x, float y, float vx, float vy) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(vx) // vel_x
    .prop<float>(vy) // vel_y
    .event<EntityUpdateNode>(lambda entity_dust_update(EntityNode* entity, TilemapNode* tilemap, float delta_time): void {
        if (entity.vel_x < 0) {
            entity.vel_x += 0.01 * delta_time;
            if (entity.vel_x > 0) entity.vel_x = 0;
        }
        if (entity.vel_x > 0) {
            entity.vel_x -= 0.01 * delta_time;
            if (entity.vel_x < 0) entity.vel_x = 0;
        }

        if (entity.vel_y < 0) {
            entity.vel_y += 0.01 * delta_time;
            if (entity.vel_y > 0) entity.vel_y = 0;
        }
        if (entity.vel_y > 0) {
            entity.vel_y -= 0.01 * delta_time;
            if (entity.vel_y < 0) entity.vel_y = 0;
        }

        *entity.prop<float>("timer") += delta_time;
        if (*entity.prop<float>("timer") >= 32) entity.node.delete();
    })
    .event<EntityTextureNode>(lambda entity_dust_texture(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h): Texture* {
        int index = *entity.prop<float>("timer") / 4;
        *srcx = index * 8;
        *srcy = 0;
        *srcw = *srch = *w = *h = 8;
        return assets.get<Texture>("images/entities/dust.png");
    })
.build();

void explode_dust(TilemapNode* tilemap, float x, float y, float vel, int amount) {
    for (int i = 0; i < amount; i++) {
        float angle = frng(0, 2 * 3.14159);
        tilemap.node.attach(entity_dust(x, y,
            vel * cosf(angle),
            vel * sinf(angle)
        ));
    }
}
