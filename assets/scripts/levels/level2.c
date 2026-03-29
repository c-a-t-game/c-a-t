#depends "scripts/entities/player.c"
#depends "scripts/entities/mouse.c"
#depends "scripts/tilesets/grass.c"
#depends "scripts/decorators/foliage.c"

Node* level2() -> engine.open<LevelRootNode>()
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
            0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,
            1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,
        })
        .attach(entity_level_end(7.5f, 14.0f))
        .attach(entity_level_start(1.5f, 14.0f))
        .attach(entity_player(1.5f, 14.0f))
    .close()
.build();