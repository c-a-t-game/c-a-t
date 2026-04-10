#depends "scripts/engine.c"
#depends "scripts/screenshake.c"
#depends "scripts/entities/heart.c"
#depends "scripts/entities/crate_fragment.c"

#depends "scripts/audio/sounds.c"

#define signum(x) ((x) == 0 ? 0 : (x) / fabsf(x))

Node* entity_wrap_controller() -> engine.open<EntityNode>()
    .prop<float>(0)
    .prop<float>(0)
    .prop<float>(0)
    .prop<float>(0)
    .prop<float>(0)
    .prop<float>(0)
    .prop<const char*>("entity_wrap_controller")
    .prop<const char*>("wrap_controller")
.build();

Node* entity_darkness_controller() -> engine.open<EntityNode>()
    .prop<float>(0)
    .prop<float>(0)
    .prop<float>(0)
    .prop<float>(0)
    .prop<float>(0)
    .prop<float>(0)
    .prop<const char*>("entity_darkness_controller")
    .event<EntityUpdateNode>(lambda entity_darkness_controller_update(EntityNode* entity, TilemapNode* tilemap, float delta_time): void {
        EntityNode* player = tilemap.find("player");
        entity.pos_x = player.pos_x;
        entity.pos_y = player.pos_y;
    })
    .event<EntityTextureNode>(lambda entity_darkness_controller_texture(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h, float* off_x, float* off_y): Texture* {
        *off_y = 280;
        return assets.get<Texture>("images/darkness.png");
    })
.build();

Node* entity_player(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(0) // vel_x
    .prop<float>(0) // vel_y
    .prop<float>(0.75) // width
    .prop<float>(0.75) // height
    .prop<const char*>("entity_player") // func
    .prop<const char*>("player") // name
    .event<EntityUpdateNode>(lambda entity_player_update(EntityNode* entity, TilemapNode* tilemap, float delta_time): void {
        *entity.prop<int>("draw_priority") = 999;
        if ((editor_is_editing() & 0xFF) && editor_noclip) {
            float speed = input.down("shift") ? 0.3 : 0.1;
            entity.vel_x = entity.vel_y = 0;
            *entity.prop<float>("cam_offset") = 0;
            if (input.down("left"))  entity.pos_x -= speed * delta_time;
            if (input.down("right")) entity.pos_x += speed * delta_time;
            if (input.down("up"))    entity.pos_y -= speed * delta_time;
            if (input.down("down"))  entity.pos_y += speed * delta_time;
        }
        else if (*entity.prop<bool>("napping")) {
            if (*entity.prop<float>("nap_timer") > 120) {
                if (engine.editor_mode()) {
                    editor_toggle_play_mode = true;
                    *entity.prop<bool>("napping") = false;
                    *entity.prop<float>("nap_timer") = 0;
                }
                else {
                    LevelRootNode* level = entity.node.parent.parent;
                    __curr_level_loader = level.next_level;
                    if (engine.create_transition(engine.reload, 60, Direction_Left))
                        sound_transition().play_oneshot();
                }
            }
            *entity.prop<float>("nap_timer") += delta_time;
        }
        else if (*entity.prop<bool>("hurt")) {
            entity.vel_y += 0.03 * delta_time;
            if (*entity.prop<bool>("touching_ground")) {
                if (*storage.get<int>("num_hearts") == 0) {
                    if (engine.editor_mode()) {
                        editor_toggle_play_mode = true;
                        *entity.prop<bool>("hurt") = false;
                    }
                    else {
                        if (engine.create_transition(engine.reload, 60, Direction_Left))
                            sound_transition().play_oneshot();
                        entity.vel_x = 0;
                    }
                }
                else {
                    (*storage.get<int>("num_hearts"))--;
                    *entity.prop<float>("invincibility_frames") = 300;
                    *entity.prop<bool>("hurt") = false;
                }
            }
        }
        else {
            entity.width = entity.height = 0.75;

            int curr_dir = signum(entity.vel_x);
            int dir = input.down("left") * -1 + input.down("right") * 1;
            bool moving = dir != 0;
            if (!moving) dir = -curr_dir;
            float accel = 0.015 * dir;
            if (curr_dir != dir) accel *= *entity.prop<bool>("touching_ground") ? 4 : moving ? 2 : 0.25;
            entity.vel_x += accel * delta_time;
            if (entity.vel_x < -0.2) entity.vel_x = -0.2;
            if (entity.vel_x >  0.2) entity.vel_x =  0.2;
            if (signum(entity.vel_x) != curr_dir && curr_dir != 0) entity.vel_x = 0;

            entity.prop<float>("cam_offset");
            if (moving && dir < 0) {
                *entity.prop<bool>("facing_left") = true;
                *entity.prop<float>("cam_offset") -= 0.1 * delta_time;
                if (*entity.prop<float>("cam_offset") < -2) *entity.prop<float>("cam_offset") = -2;
            }
            if (moving && dir > 0) {
                *entity.prop<bool>("facing_left") = false;
                *entity.prop<float>("cam_offset") += 0.1 * delta_time;
                if (*entity.prop<float>("cam_offset") > 2) *entity.prop<float>("cam_offset") = 2;
            }

            if (!*entity.prop<bool>("first_frame_flag")) {
                *entity.prop<float>("jump_buffer_timer") = 999;
                *entity.prop<bool>("first_frame_flag") = true;
            }

            if (*entity.prop<float>("time_until_death") > 0) {
                if (editor_is_editing()) {
                    *entity.prop<float>("time_until_death") = 0;
                    *entity.prop<bool>("death_timer_enabled") = false;
                }
                *entity.prop<float>("time_until_death") -= delta_time;
                *entity.prop<bool>("death_timer_enabled") = true;
            }
            if (*entity.prop<bool>("death_timer_enabled") && *entity.prop<float>("time_until_death") <= 0) {
                *storage.get<int>("num_hearts") = 0;
                *entity.prop<bool>("hurt") = true;
                *entity.prop<bool>("touching_ground") = false;
                *entity.prop<bool>("death_timer_enabled") = false;
                entity.vel_y = -0.3;
            }

            entity.vel_y += 0.03 * delta_time;

            *entity.prop<float>("jump_buffer_timer") += delta_time;
            *entity.prop<float>("coyote_timer") += delta_time;
            *entity.prop<float>("invincibility_frames") -= delta_time;
            if (*entity.prop<float>("invincibility_frames") < 0 || editor_is_editing())
                *entity.prop<float>("invincibility_frames") = 0;

            if (!input.down("jump")) *entity.prop<bool>("jumping") = false;
            if (input.pressed("jump")) *entity.prop<float>("jump_buffer_timer") = 0;
            if (*entity.prop<bool>("touching_ground")) {
                *entity.prop<float>("coyote_timer") = 0;
                *entity.prop<bool>("jumped") = false;
            }
            if (!*entity.prop<bool>("jumped") && *entity.prop<float>("coyote_timer") <= 5 && *entity.prop<float>("jump_buffer_timer") <= 5) {
                sound_jump().play_oneshot();
                *entity.prop<float>("squish") -= 15;
                *entity.prop<bool>("jumping") = *entity.prop<bool>("jumped") = true;
                *entity.prop<float>("floor_y") = entity.pos_y;
            }
            if (*entity.prop<bool>("jumping")) {
                if (*entity.prop<float>("floor_y") - entity.pos_y > 3 ||
                    *entity.prop<Direction>("ver_collision") == Direction_Up
                ) *entity.prop<bool>("jumping") = false;
                else entity.vel_y = -0.3;
            }

            float* scratch_timer = entity.prop<float>("scratch_timer");
            *scratch_timer -= delta_time;
            if (*scratch_timer < -15) *scratch_timer = -15;
            if (input.pressed("attack") && *scratch_timer == -15 && !editor_is_editing()) {
                TilemapNode* tilemap = entity.node.parent;
                void(*break_crate)(EntityNode*, float, float) = lambda(EntityNode* entity, float off_x, float off_y): void {
                    TilemapNode* tilemap = entity.node.parent;
                    int x = entity.pos_x + off_x;
                    int y = entity.pos_y + off_y;
                    if (tilemap.get(x, y) == 4) {
                        tilemap.set(x, y, 0);
                        for (int i = 0; i < 4; i++) tilemap.node.attach(entity_crate_fragment(x, y));
                        sound_break().play_oneshot();
                        screenshake += 15;
                    }
                };
                break_crate(entity, 0, 0.5);
                break_crate(entity, *entity.prop<bool>("facing_left") ? -0.5 : 0.5, -0.1);
                for (int i = 0; i < tilemap.node.children_size; i++) {
                    if (tilemap.node.children[i] && (int)tilemap.node.children[i].type == NodeType_Entity && tilemap.node.children[i] != entity) {
                        EntityNode* scratchee /* this fuckass name lmfao */ = tilemap.node.children[i];
                        if (
                            ( *entity.prop<bool>("facing_left") && scratchee.pos_x < entity.pos_x) ||
                            (!*entity.prop<bool>("facing_left") && scratchee.pos_x > entity.pos_x)
                        ) {
                            float max_dist = fmax(fabsf(entity.vel_x) * 15, 1.75);
                            float dist = sqrtf(
                                (scratchee.pos_x - entity.pos_x) * (scratchee.pos_x - entity.pos_x) +
                                (scratchee.pos_y - entity.pos_y) * (scratchee.pos_y - entity.pos_y)
                            );
                            if (dist < max_dist) scratchee.damage(entity);
                        }
                    }
                }
                *scratch_timer = 12;
            }
            if (tilemap.find("wrap_controller")) {
                if (entity.pos_y > tilemap.end_y + entity.height) entity.pos_y = tilemap.start_y - entity.height;
                else if (entity.pos_y < tilemap.start_y - entity.height) {
                    float prev_y = entity.pos_y;
                    entity.pos_y = tilemap.end_y + entity.height;
                    *entity.prop<float>("floor_y") += entity.pos_y - prev_y - entity.height;
                }
            }
            else if (entity.pos_y > tilemap.end_y + 4) {
                if (engine.editor_mode()) {
                    if (!editor_is_editing()) editor_toggle_play_mode = true;
                    editor_noclip = true;
                    entity.pos_y = tilemap.end_y - 2;
                }
                else if (engine.create_transition(engine.reload, 60, Direction_Left))
                    sound_transition().play_oneshot();
            }
            *entity.prop<bool>("squashed") = false;

            float* squish = entity.prop<float>("squish");
            if (*squish < -1) *squish = -1;
            if (*squish > +1) *squish = +1;
            if (*squish < 0) {
                *squish += delta_time;
                if (*squish > 0) *squish = 0;
            }
            if (*squish > 0) {
                *squish -= delta_time;
                if (*squish < 0) *squish = 0;
            }
        }
        *entity.prop<float>("cam_target") = (entity.pos_x + *entity.prop<float>("cam_offset")) * 16 - 192;
        if (!editor_is_editing()) {
            if (*entity.prop<float>("cam_target") > tilemap.end_x * 16 - 384) *entity.prop<float>("cam_target") = tilemap.end_x * 16 - 384;
            if (*entity.prop<float>("cam_target") < tilemap.start_x * 16)     *entity.prop<float>("cam_target") = tilemap.start_x * 16;
        }
        float ss_x, ss_y;
        process_screenshake(&ss_x, &ss_y, delta_time);
        *entity.prop<float>("cam_x") += (*entity.prop<float>("cam_target") - ((LevelRootNode*)tilemap.node.parent).cam_x) / 10 * delta_time;
        engine.level().cam_x = *entity.prop<float>("cam_x") + ss_x;
        engine.level().cam_y = *entity.prop<float>("cam_y") + ss_y;
    })
    .event<EntityDamageNode>(lambda entity_player_damage(EntityNode* entity, EntityNode* source, TilemapNode* tilemap): void {
        if (*entity.prop<bool>("hurt")) return;
        if (*entity.prop<float>("invincibility_frames") > 0) return;
        *entity.prop<bool>("hurt") = true;
        *entity.prop<bool>("touching_ground") = false;
        entity.vel_y = -0.3;
        entity.pos_y -= 0.1;
        if (entity.pos_x < source.pos_x) {
            *entity.prop<bool>("facing_left") = false;
            entity.vel_x = -0.15;
        }
        else {
            *entity.prop<bool>("facing_left") = true;
            entity.vel_x = 0.15;
        }
        if (*storage.get<int>("num_hearts") != 0) {
            entity.node.parent.attach(entity_broken_heart(entity.pos_x, entity.pos_y, -0.1));
            entity.node.parent.attach(entity_broken_heart(entity.pos_x, entity.pos_y, +0.1));
        }
        sound_get_hurt().play_oneshot();
        screenshake += 30;
    })
    .event<EntityTextureNode>(lambda entity_player_texture(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h): Texture* {
        int sprite = 0;
        if      (*entity.prop<bool>("napping")) sprite = (int[]){ 11, 12 }[engine.get_millis() / 300 % 2];
        else if (*entity.prop<bool>("hurt")) sprite = 8;
        else if (*entity.prop<float>("scratch_timer") > 6) sprite = 9;
        else if (*entity.prop<float>("scratch_timer") > 0) sprite = 10;
        else if  (entity.vel_y >  0) sprite = 7;
        else if  (entity.vel_y <  0) sprite = 6;
        else if  (entity.vel_x != 0) sprite = (int)(entity.pos_x * 1) % 2 + 4;
        else sprite = (int[]){ 0, 1, 2, 3, 2, 1 }[engine.get_millis() / 150 % 6];

        editor_push_trail(entity.pos_x, entity.pos_y, sprite, *entity.prop<bool>("facing_left"));

        *srcx = sprite * 16;
        *srcy = 0;

        bool visible = (int)*entity.prop<float>("invincibility_frames") % 2 == 0;
        *w = *entity.prop<bool>("facing_left") ? -16 : 16;
        *srcw = *srch = *h = 16;

        float squish =  *entity.prop<float>("squish") / 30;
        if (squish > +1) squish = +1;
        if (squish < -1) squish = -1;
        *w += squish * 16 * (*w < 0 ? -1 : 1);
        *h -= squish * 16;

        return visible ? assets.get<Texture>("images/entities/player.png") : nullptr;
    })
    .event<CollisionNode>(lambda entity_player_collision(EntityNode* entity, TilemapNode* tilemap, TileNode* tile, int x, int y, int direction): void {
        if (direction != Direction_Down) return;
        if (*entity.prop<Direction>("ver_collision") != Direction_Down) return;
        if (entity.vel_y < 0.1) return;
        *entity.prop<float>("squish") += 15 * entity.vel_y;
    })
.build();
