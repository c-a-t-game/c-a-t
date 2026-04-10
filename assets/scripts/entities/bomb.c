#include <math.h>

#depends "scripts/entities/heart.c"

#define EPSILON (0.01)

Node* entity_bomb(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(0) // vel_x
    .prop<float>(0) // vel_y
    .prop<float>(0) // width
    .prop<float>(0) // height
    .prop<const char*>("entity_bomb") // func
    .event<EntityUpdateNode>(entity_heart_update)
    .event<EntityUpdateNode>(lambda entity_bomb_update(EntityNode* entity, TilemapNode* tilemap, float delta_time): void {
        if (tilemap.get(entity.pos_x, entity.pos_y - EPSILON) != 4 && !editor_is_editing()) {
            *entity.prop<float>("bomb_timer") += delta_time;
        }
        if (tilemap.get(entity.pos_x, entity.pos_y - EPSILON) == 4) {
            *entity.prop<int>("crate_x") = entity.pos_x;
            *entity.prop<int>("crate_y") = entity.pos_y - EPSILON;
            *entity.prop<bool>("in_crate") = true;
        }
        if (*entity.prop<bool>("touching_ground")) {
            if (*entity.prop<bool>("just_scratched"))
                *entity.prop<bool>("just_scratched") = false;
            else entity.vel_x = 0;
        }
        if (*entity.prop<float>("bomb_timer") >= 600) {
            screenshake += 30;
            sound_explosion().play_oneshot();
            int orig_x = floorf(entity.pos_x);
            int orig_y = floorf(entity.pos_y - entity.height / 2);
            bool did_damage = false;
            for (int y = -2; y <= 2; y++) for (int x = -2; x <= 2; x++) if (abs(x) + abs(y) != 4) {
                if (tilemap.get(orig_x + x, orig_y + y) == 13) {
                    tilemap.set(orig_x + x, orig_y + y, 0);
                    did_damage = true;
                }
            }
            for (int i = 0; i < tilemap.node.children_size; i++) {
                if (tilemap.node.children[i] && (int)tilemap.node.children[i].type == NodeType_Entity && tilemap.node.children[i] != entity) {
                    EntityNode* e = tilemap.node.children[i];
                    if (sqrtf((entity.pos_x - e.pos_x) * (entity.pos_x - e.pos_x) + (entity.pos_y - e.pos_y) * (entity.pos_y - e.pos_y)) < 2.5) {
                        e.damage(entity);
                        did_damage = true;
                    }
                }
            }
            if (!did_damage && *entity.prop<bool>("in_crate")) {
                tilemap.set(*entity.prop<int>("crate_x"), *entity.prop<int>("crate_y"), 4);
                tilemap.node.attach(entity_bomb(*entity.prop<int>("crate_x") + 0.5, *entity.prop<int>("crate_y") + 1));
            }
            entity.node.delete();
        }
    })
    .event<EntityDamageNode>(lambda entity_bomb_damage(EntityNode* entity, EntityNode* source, TilemapNode* tilemap): void {
        if (editor_is_editing()) return;
        if (tilemap.get(entity.pos_x, entity.pos_y - EPSILON) == 4) return;
        entity.vel_x = entity.pos_x < source.pos_x ? -0.15 : 0.15;
        entity.vel_y = -0.2;
        *entity.prop<bool>("just_scratched") = true;
    })
    .event<EntityTextureNode>(lambda entity_bomb_texture(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h): Texture* {
        if (tilemap.get(entity.pos_x, entity.pos_y - EPSILON) == 4 && !editor_is_editing()) return nullptr;
        float x = *entity.prop<float>("bomb_timer") / 600;
        x = round(fabsf(sin(M_PI * x * x * 64)));
        *srcx = *entity.prop<float>("bomb_timer") > 540 ? 16 : x * 16;
        *srcy = 0;
        *srcw = *srch = *w = *h = 16;
        if (editor_is_editing()) *srcx = 0;
        return assets.get<Texture>("images/entities/bomb.png");
    })
.build();
