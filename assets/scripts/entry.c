#depends "scripts/levels/level1.c"

#depends "scripts/engine.c"
#depends "scripts/ui.c"

void entry_point() {
    input.add("left", 4);
    input.add("right", 7);
    input.add("up", 26);
    input.add("down", 22);
    input.add("jump", 44);
    input.add("shift", 225);
    input.add("ctrl", 224);

    input.add("editor_picker", 8);
    input.add("editor_select", 30);
    input.add("editor_pencil", 31);
    input.add("editor_eraser", 32);
    input.add("editor_tilemap", 33);
    input.add("editor_object", 34);

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

        if (editor_is_editing()) editor_update();
        else if (engine.editor_mode()) editor_play_button();
        else ui_hud();

        w.set_buffer(nullptr);
        w.blit(buf, 0, 0, 1152, 768, 0, 0, 384, 256, 0xFFFFFFFF);
        w.end_frame();

        buf.destroy();
    }
    w.close();
}
