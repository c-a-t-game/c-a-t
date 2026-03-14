#depends "scripts/entities/player.c"
#depends "scripts/entities/mouse.c"
#depends "scripts/entities/hud.c"

#depends "scripts/tilesets/grass.c"

#depends "scripts/decorators/foliage.c"

Tile level1_tiles[24*16] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,0,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,7,7,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,6,6,0,5,5,5,0,0,0,0,6,6,0,0,0,
    0,0,0,0,0,0,0,0,0,6,6,7,7,7,7,7,0,0,0,6,6,0,0,0,
    0,0,0,0,0,0,0,0,0,6,6,0,0,0,0,0,0,0,0,6,6,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,7,7,6,6,0,0,0,
    0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,6,6,0,0,0,
    0,0,0,0,0,0,0,4,4,0,0,0,0,0,0,0,0,0,0,6,6,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

Node* level1() -> engine.open<LevelRootNode>()
    .exec(grass_bg)
    .open<TilemapNode>()
        .attach(tileset_grass())
        .prop<float>(1.0f) // scale_x
        .prop<float>(1.0f) // scale_y
        .prop<float>(0.0f) // scroll_offset_x
        .prop<float>(0.0f) // scroll_offset_y
        .prop<float>(1.0f) // scroll_speed_x
        .prop<float>(1.0f) // scroll_speed_y
        .prop<int>(24) // width
        .prop<int>(16) // height
        .prop<Tile>(0) // oob_tile
        .prop<Tile*>(level1_tiles) // tiles
        .attach(entity_player(1.5, 14))
        .attach(entity_mouse(20, 7))
        .attach(entity_hud())
    .close().decorate(foliage_decorator)
.build();
