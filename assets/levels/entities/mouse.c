#depends "levels/engine.c"

Node* entity_mouse(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(0) // vel_x
    .prop<float>(0) // vel_y
    .prop<float>(0.75) // width
    .prop<float>(0.75) // height
    .event<EntityUpdateNode>(lambda entity_mouse_update(EntityNode* entity, TilemapNode* tilemap, float delta_time): void {
        EntityNode* player = tilemap.find("player");
        if (!player) return;

        if (*entity.prop<Direction>("last_hor_collision") == Direction_None && player && fabsf(entity.pos_x - player.pos_x) < 16)
            *entity.prop<Direction>("last_hor_collision") = Direction_Right;
        entity.vel_x = 0.05 * (
            *entity.prop<Direction>("last_hor_collision") == Direction_Left ? 1 :
            *entity.prop<Direction>("last_hor_collision") == Direction_None ? 0 : -1
        );
        entity.vel_y += 0.025 * delta_time;
    })
    .event<EntityCollisionNode>(lambda entity_mouse_collision(EntityNode* collidee, EntityNode* collider): void {
        if (!collider.is("player")) return;
        if (collider.vel_y > 0) {
            collider.vel_y = -0.3;
            *collider.prop<bool>("jumping") = true;
            *collider.prop<float>("floor_y") = collider.pos_y;
            collidee.node.delete();
        }
    })
    .event<EntityTextureNode>(lambda entity_mouse_texture(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h): Texture* {
        *srcx = 0;//16 * (int)(engine.get_millis() % (200 * 2) / 200);
        *srcy = 0;
        *srcw = *srch = *h = 16;
        *w = *entity.prop<Direction>("last_hor_collision") == Direction_Left ? 16 : -16;
        return assets.get<Texture>("trolley.png");
    })
.build();
