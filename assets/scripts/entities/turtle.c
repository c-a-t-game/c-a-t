#depends "scripts/entities/player.c"
#depends "scripts/entities/crate_fragment.c"

Node* entity_turtle_shell_fragment(float x, float y, float mul) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(frng( 0.2, 0.05) * mul) // vel_x
    .prop<float>(frng(-0.4, -0.2))       // vel_y
    .prop<float>(0.5) // width
    .prop<float>(0.5) // height
    .event<EntityUpdateNode>(lambda(EntityNode* entity): void -> *entity.prop<Texture*>("texture") = assets.get<Texture>("images/entities/turtle_shell_fragment.png"))
    .event<EntityUpdateNode>(entity_crate_fragment_update)
    .event<EntityTextureNode>(entity_crate_fragment_texture)
.build();

Node* entity_turtle_shell(float x, float y, float v) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(v) // vel_x
    .prop<float>(-0.2) // vel_y
    .prop<float>(0.75) // width
    .prop<float>(0.75) // height
    .prop<const char*>("entity_turtle_shell") // func
    .prop<const char*>("shell") // name
    .event<EntityUpdateNode>(lambda entity_turtle_shell_update(EntityNode* entity, TilemapNode* tilemap, float delta_time): void {
        if (*entity.prop<Direction>("hor_collision") != Direction_None) {
            if (*entity.prop<bool>("nobreak")) *entity.prop<bool>("nobreak") = false;
            else {
                for (int i = 0; i < 4; i++) {
                    entity.node.parent.attach(entity_turtle_shell_fragment(entity.pos_x, entity.pos_y, *entity.prop<Direction>("last_hor_collision") == Direction_Left ? 1 : -1));
                }
                entity.node.delete();
            }
        }
        *entity.prop<float>("intangible") -= delta_time;
        if (*entity.prop<float>("intangible") < 0) *entity.prop<float>("intangible") = 0;
        entity.vel_y += 0.025 * delta_time;
    })
    .event<EntityCollisionNode>(lambda entity_turtle_shell_collision(EntityNode* collidee, EntityNode* collider, TilemapNode* tilemap): void {
        if (collidee.vel_x == 0) {
            if (!collider.is("player")) return;
            *collidee.prop<float>("intangible") = 30;
            collidee.vel_x = (collidee.pos_x < collider.pos_x ? -1 : 1) * 0.15;
        }
        else if (*collidee.prop<float>("intangible") == 0 || !collider.is("player"))
            collider.damage(collidee);
    })
    .event<EntityDamageNode>(lambda entity_turtle_shell_damage(EntityNode* entity, EntityNode* source, TilemapNode* tilemap): void {
        if (*entity.prop<float>("intangible") > 0) return;
        if (entity.vel_x == 0) entity.vel_x = entity.pos_x < source.pos_x ? -0.15 : 0.15;
        else if (entity.vel_x < 0 == entity.pos_x < source.pos_x) entity.vel_x *= 2;
        else entity.vel_x = 0;
        entity.vel_y = -0.2;
    })
    .event<EntityTextureNode>(lambda entity_turtle_shell_texture(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h): Texture* {
        *srcx = ((int)(entity.pos_x * 1) % 3 + 3) * 16;
        *srcy = 0;
        *srcw = *srch = 16;
        *w = *entity.prop<Direction>("last_hor_collision") == Direction_Left ? 16 : -16;
        *h = -16;
        return assets.get<Texture>("images/entities/turtle.png");
    })
.build();

Node* entity_turtle(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(0) // vel_x
    .prop<float>(0) // vel_y
    .prop<float>(0.75) // width
    .prop<float>(0.75) // height
    .prop<const char*>("entity_turtle") // func
    .event<EntityUpdateNode>(lambda entity_turtle_update(EntityNode* entity, TilemapNode* tilemap, float delta_time): void {
        if (editor_is_editing()) return;
        EntityNode* player = tilemap.find("player");
        if (*entity.prop<Direction>("last_hor_collision") == Direction_None && player && fabsf(entity.pos_x - player.pos_x) < 16)
            *entity.prop<Direction>("last_hor_collision") = Direction_Right;
        entity.vel_x = 0.05 * (
            *entity.prop<Direction>("last_hor_collision") == Direction_Left ? 1 :
            *entity.prop<Direction>("last_hor_collision") == Direction_None ? 0 : -1
        );
        entity.vel_y += 0.025 * delta_time;
    })
    .event<EntityCollisionNode>(lambda entity_turtle_collision(EntityNode* collidee, EntityNode* collider, TilemapNode* tilemap): void {
        if (editor_is_editing()) return;
        if (!collider.is("player")) return;
        collider.damage(collidee);
    })
    .event<EntityDamageNode>(lambda entity_turtle_damage(EntityNode* entity, EntityNode* source, TilemapNode* tilemap): void {
        EntityNode* shell = entity_turtle_shell(entity.pos_x, entity.pos_y, entity.pos_x < source.pos_x ? -0.15 : 0.15);
        *shell.prop<float>("intangible") = 30;
        entity.node.parent.attach(shell);
        entity.node.delete();
    })
    .event<EntityTextureNode>(lambda entity_turtle_texture(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h): Texture* {
        *srcx = 16 * (int)(engine.get_millis() % (200 * 2) / 200);
        *srcy = 0;
        *srcw = *srch = *h = 16;
        *w = *entity.prop<Direction>("last_hor_collision") == Direction_Left ? 16 : -16;
        return assets.get<Texture>("images/entities/turtle.png");
    })
.build();
