#depends "levels/engine.c"

#define signum(x) ((x) == 0 ? 0 : (x) / fabsf(x))

Node* entity_player(float x, float y) -> engine.open<EntityNode>()
    .prop<float>(x) // pos_x
    .prop<float>(y) // pos_y
    .prop<float>(0) // vel_x
    .prop<float>(0) // vel_y
    .prop<float>(0.75) // width
    .prop<float>(0.75) // height
    .prop<const char*>("player") // name
    .event<EntityUpdateNode>(lambda entity_player_update(EntityNode* entity, TilemapNode* tilemap, float delta_time): void {
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

        *entity.prop<float>("cam_target") = (entity.pos_x + *entity.prop<float>("cam_offset")) * 16 - 192;
        ((LevelRootNode*)tilemap.node.parent).cam_x += (*entity.prop<float>("cam_target") - ((LevelRootNode*)tilemap.node.parent).cam_x) / 10 * delta_time;

        if (!*entity.prop<bool>("first_frame_flag")) {
            *entity.prop<float>("jump_buffer_timer") = 999;
            *entity.prop<bool>("first_frame_flag") = true;
        }

        entity.vel_y += 0.03 * delta_time;

        *entity.prop<float>("jump_buffer_timer") += delta_time;
        *entity.prop<float>("coyote_timer") += delta_time;

        if (!input.down("jump")) *entity.prop<bool>("jumping") = false;
        if (input.pressed("jump")) *entity.prop<float>("jump_buffer_timer") = 0;
        if (*entity.prop<bool>("touching_ground")) {
            *entity.prop<float>("coyote_timer") = 0;
            *entity.prop<bool>("jumped") = false;
        }
        if (!*entity.prop<bool>("jumped") && *entity.prop<float>("coyote_timer") <= 5 && *entity.prop<float>("jump_buffer_timer") <= 5) {
            *entity.prop<bool>("jumping") = *entity.prop<bool>("jumped") = true;
            *entity.prop<float>("floor_y") = entity.pos_y;
        }
        if (*entity.prop<bool>("jumping")) {
            if (*entity.prop<float>("floor_y") - entity.pos_y > 3 ||
                *entity.prop<Direction>("ver_collision") == Direction_Up
            ) *entity.prop<bool>("jumping") = false;
            else entity.vel_y = -0.3;
        }
        
        if (input.pressed("reload")) engine.reload();
    })
    .event<EntityTextureNode>(lambda entity_player_texture(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h): Texture* {
        int sprite = 0;
        if      (entity.vel_y >  0) sprite = 7;
        else if (entity.vel_y <  0) sprite = 6;
        else if (entity.vel_x != 0) sprite = (int)(entity.pos_x * 1) % 2 + 4;
        else sprite = (int[]){ 0, 1, 2, 3, 2, 1 }[engine.get_millis() % (6 * 150) / 150];

        *srcx = sprite * 16;
        *srcy = 0;

        *w = *entity.prop<bool>("facing_left") ? -16 : 16;
        *srcw = *srch = *h = 16;
        return assets.get<Texture>("player.png");
    })
.build();
