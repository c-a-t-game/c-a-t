#depends "scripts/editor.c"

#define EPSILON (0.01)

Node* entity_heart(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(0) // vel_x
    .prop<float>(0) // vel_y
    .prop<float>(0) // width
    .prop<float>(0) // height
    .prop<const char*>("entity_heart") // func
    .event<EntityUpdateNode>(lambda(EntityNode* entity, TilemapNode* tilemap, float delta_time): void {
        *entity.prop<Texture*>("texture") = assets.get<Texture>("images/entities/heart.png");
        *entity.prop<char*>("target_storage") = "num_hearts";
    })
    .event<EntityUpdateNode>(lambda entity_heart_update(EntityNode* entity, TilemapNode* tilemap, float delta_time): void {
        if (editor_is_editing()) {
            entity.width = entity.height = 1;
            return;
        }
        else entity.width = entity.height = 0;
        if (tilemap.get(entity.pos_x, entity.pos_y - EPSILON) != 4) {
            entity.width = entity.height = 1;
            float* intangible = entity.prop<float>("intangible");
            if (!*entity.prop<bool>("bounced")) {
                *entity.prop<bool>("bounced") = true;
                *entity.prop<float>("intangible") = 30;
                entity.vel_y = -0.3;
            }
            entity.vel_y += 0.02 * delta_time;
            *entity.prop<float>("intangible") -= delta_time;
        }
    })
    .event<EntityTextureNode>(lambda entity_heart_texture(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h): Texture* {
        if (!editor_is_editing()) if (tilemap.get(entity.pos_x, entity.pos_y - EPSILON) == 4) return nullptr;
        Texture* tex = *entity.prop<Texture*>("texture");
        *srcx = (engine.get_millis() / 150 % (int)(tex.width / tex.height)) * tex.height;
        *srcy = 0;
        *srcw = *srch = *w = *h = tex.height;
        return tex;
    })
    .event<EntityCollisionNode>(lambda entity_heart_collision(EntityNode* collidee, EntityNode* collider, TilemapNode* tilemap): void {
        if (editor_is_editing()) return;
        if (!collider.is("player")) return;
        if (tilemap.get(collider.pos_x, collider.pos_y - EPSILON) == 4) return;
        if (*collidee.prop<float>("intangible") > 0) return;
        if (*collidee.prop<char*>("target_storage"))
            *storage.get<int>(*collidee.prop<char*>("target_storage")) += 1;
        collidee.node.delete();
    })
.build();

Node* entity_broken_heart(float x, float y, float speed) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(speed) // vel_x
    .prop<float>(-0.25) // vel_y
    .prop<float>(0) // width
    .prop<float>(0) // height
    .event<EntityUpdateNode>(lambda entity_broken_heart_update(EntityNode* entity, TilemapNode* tilemap, float delta_time): void {
        *entity.prop<float>("timer") += delta_time;
        if (*entity.prop<float>("timer") > 300) entity.node.delete();
        entity.vel_y += 0.03 * delta_time;
    })
    .event<EntityTextureNode>(lambda entity_broken_heart_texture(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h): Texture* {
        *srcx = (entity.vel_x > 0) * 16;
        *srcy = 0;
        *srcw = *srch = *w = *h = 16;
        return assets.get<Texture>("images/entities/broken_heart.png");
    })
.build();
