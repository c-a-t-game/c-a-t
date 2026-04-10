#depends "scripts/levels/level1.c"

Node* level_title() -> engine.open<LevelRootNode>()
    .prop<Level>(level1) // next_level
    .exec(grass_bg)
    .open<TilemapNode>()
        .attach(tileset_grass())
        .prop<float>(1.0f) // scale_x
        .prop<float>(1.0f) // scale_y
        .prop<float>(0.0f) // scroll_offset_x
        .prop<float>(0.0f) // scroll_offset_y
        .prop<float>(1.0f) // scroll_speed_x
        .prop<float>(1.0f) // scroll_speed_y
        .tilemap(24, 16, grass_oob_provider, (Tile[]){
            0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
            0,0,0,0,0,6,6,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,
            0,0,7,7,7,6,6,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,6,6,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,7,6,6,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,0,0,0,6,6,0,0,
            1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,0,0,0,6,6,7,0,
            1,1,1,1,0,0,0,0,0,0,0,7,7,7,7,6,6,0,0,7,6,6,0,0,
            1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,6,6,0,0,0,6,6,1,1,
            1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,6,6,0,1,1,1,1,1,1,
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        })
        .open<EntityNode>()
            .prop<float>(12.f)
            .prop<float>(10.f)
            .event<EntityUpdateNode>(lambda title_update(): void {
                if (!input.pressed("jump")) return;
                __curr_level_loader = engine.level().next_level;
                if (engine.create_transition(engine.reload, 60, Direction_Left))
                    sound_transition().play_oneshot();
            })
            .event<EntityTextureNode>(lambda title_logo(): Texture* {
                float off = sinf((engine.get_millis() % 5000) / 5000.f * 2 * 3.14159) * 4;
                ui_scaled_label(128, 194 + off, 0x3F3F3FFF, 2, "Press SPACE");
                ui_scaled_label(126, 192 + off, 0xFFFFFFFF, 2, "Press SPACE");
                return assets.get<Texture>("images/logo.png");
            })
        .close()
    .close()
.build();
