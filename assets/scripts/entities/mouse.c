#depends "scripts/entities/player.c"
#depends "scripts/audio/sounds.c"

Node* entity_squished_mouse(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(0) // vel_x
    .prop<float>(0) // vel_y
    .prop<float>(0.75) // width
    .prop<float>(0.75) // height
    .prop<const char*>("entity_squished_mouse") // func
    .event<EntityUpdateNode>(lambda entity_squished_mouse_update(EntityNode* entity, TilemapNode* tilemap, float delta_time): void {
        if (*entity.prop<float>("timer") >= 60) entity.node.delete();
        *entity.prop<float>("timer") += delta_time;
    })
    .event<EntityTextureNode>(lambda entity_squished_mouse_texture(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h): Texture* {
        *srcx = 32;
        *srcy = 0;
        *srcw = *srch = *w = *h = 16;

        float squish = (30 - *entity.prop<float>("timer")) / 30;
        if (squish < 0) squi sh = 0;
        *w += squish * 12;
        *h -= squish * 12;

        return assets.get<Texture>("images/entities/mouse.png");
    })
.build();

Node* entity_mouse(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(0) // vel_x
    .prop<float>(0) // vel_y
    .prop<float>(0.75) // width
    .prop<float>(0.75) // height
    .prop<const char*>("entity_mouse") // func
    .event<EntityUpdateNode>(lambda entity_mouse_update(EntityNode* entity, TilemapNode* tilemap, float delta_time): void {
        if (editor_is_editing()) return;

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
    .event<EntityCollisionNode>(lambda entity_mouse_collision(EntityNode* collidee, EntityNode* collider, TilemapNode* tilemap): void {
        if (editor_is_editing()) return;
        if (!collider.is("player")) return;
        if (collider.vel_y > 0 || *collider.prop<bool>("squashed")) {
            *collider.prop<bool>("squashed") = true;
            *collider.prop<bool>("jumping") = true;
            *collider.prop<float>("floor_y") = collider.pos_y;
            collider.vel_y = -0.3;
            collidee.damage(collider);
        }
        else collider.damage(collidee);
    })
    .event<EntityDamageNode>(lambda entity_mouse_damage(EntityNode* entity, EntityNode* source, TilemapNode* tilemap): void {
        entity.node.parent.attach(entity_squished_mouse(entity.pos_x, entity.pos_y));
        entity.node.delete();
        sound_stomp().play_oneshot();
        screenshake += 8;
    })
    .event<EntityTextureNode>(lambda entity_mouse_texture(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h): Texture* {
        *srcx = 16 * (int)(engine.get_millis() % (200 * 2) / 200);
        *srcy = 0;
        *srcw = *srch = *h = 16;
        *w = *entity.prop<Direction>("last_hor_collision") == Direction_Left ? 16 : -16;
        return assets.get<Texture>("images/entities/mouse.png");
    })
.build();
