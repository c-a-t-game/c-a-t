#depends "scripts/levels/level1.c"

#depends "scripts/engine.c"
#depends "scripts/ui.c"

#define KB_LETTER(l) ((l) - 'A' + 4)
#define KB_NUMBER(n) ((n) - '1' + 30)
#define KB_TAB 43
#define KB_SPACE 44
#define KB_CTRL 224
#define KB_SHIFT 225

void entry_point() {
    input.add("left", KB_LETTER('A'));
    input.add("right", KB_LETTER('D'));
    input.add("up", KB_LETTER('W'));
    input.add("down", KB_LETTER('S'));
    input.add("attack", KB_LETTER('L'));
    input.add("jump", KB_SPACE);
    input.add("shift", KB_SHIFT);
    input.add("ctrl", KB_CTRL);

    input.add("editor_tile_picker", KB_LETTER('E'));
    input.add("editor_obj_picker", KB_LETTER('Q'));
    input.add("editor_select", KB_NUMBER('1'));
    input.add("editor_pencil", KB_NUMBER('2'));
    input.add("editor_eraser", KB_NUMBER('3'));
    input.add("editor_mode_toggle", KB_NUMBER('4'));
    input.add("editor_play", KB_TAB);

    if (storage.num_slots() == 0) storage.use(storage.add());
    else storage.load(0);

    if (engine.editor_mode()) {
        engine.load(lambda(): Node* -> engine.open<LevelRootNode>()
            .exec(grass_bg)
            .open<TilemapNode>()
                .attach(tileset_grass())
                .prop<float>(1.0f) // scale_x
                .prop<float>(1.0f) // scale_y
                .prop<float>(0.0f) // scroll_offset_x
                .prop<float>(0.0f) // scroll_offset_y
                .prop<float>(1.0f) // scroll_speed_x
                .prop<float>(1.0f) // scroll_speed_y
                .tilemap(16, 16, 0, (Tile[]){
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                })
                .attach(entity_player(0.5, 15))
            .close()
        .build());
        editor_init();
    }
    else engine.load(level1);

    __curr_transition.progress = 1;

    Window* w = gfx.open(":3", 1152, 768);
    w.set_active();

    uint64_t last_micros = engine.get_micros();
    while (!gfx.should_close()) {
        uint64_t curr_micros = engine.get_micros();
        float delta_time = (curr_micros - last_micros) / 1000000.f * 60;
        last_micros = curr_micros;

        w.start_frame();
        Buffer* buf = w.new_buffer(384, 256);
        w.set_buffer(buf);

        input.update();
        engine.level().update(delta_time);
        engine.level().render(384, 256);
        engine.cleanup();

        float offset_x = 0, offset_y = 0;
        if (__curr_transition.progress < 1) {
            float prev_progress = __curr_transition.progress, x = -32, y = -32;
            float t = __curr_transition.progress += delta_time / __curr_transition.time;
            t = t < 0.5 ? 4 * t * t * t : 1 - (-2 * t + 2) * (-2 * t + 2) * (-2 * t + 2) / 2; // cubic inout
            if (prev_progress < 0.5 && __curr_transition.progress >= 0.5) {
                void(*func)() = __curr_transition.func;
                func();
            }
            if (__curr_transition.direction == Direction_Up)    y = -576 * t + 256, offset_y = t * -512 + (t < 0.5 ? 0 : +512);
            if (__curr_transition.direction == Direction_Left)  x = -832 * t + 384, offset_x = t * -768 + (t < 0.5 ? 0 : +768);
            if (__curr_transition.direction == Direction_Down)  y =  576 * t - 320, offset_y = t * +512 + (t < 0.5 ? 0 : -512);
            if (__curr_transition.direction == Direction_Right) x =  832 * t - 448, offset_x = t * +768 + (t < 0.5 ? 0 : -768);
            gfx.main().draw(assets.get<Texture>("images/transition.png"), x, y, 448, 320, 0, 0, 448, 320, 0xFFFFFFFF);
        }

        if (editor_is_editing()) editor_update();
        else if (engine.editor_mode()) {
            editor_play_button();
            gfx.main().draw(assets.get<Texture>("images/hud/cursors.png"), input.mouse_x() - 7, input.mouse_y() - 1, 14, 14, 28, 28, 14, 14, 0xFFFFFFFF);
        }
        else ui_hud();

        w.set_buffer(nullptr);
        w.blit(buf, offset_x * 3, offset_y * 3, 1152, 768, 0, 0, 384, 256, 0xFFFFFFFF);
        w.end_frame();

        buf.destroy();

        engine.check_watched_files();
    }
    w.close();
}
