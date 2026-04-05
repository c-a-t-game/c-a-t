#depends "scripts/tilesets/grass.c"

Node* tileset_cave() -> engine.open<TilesetNode>()
    .prop<Texture*>(assets.get<Texture>("images/tilesets/cave.png"))
    .exec(tileset_grass_data)
.build();

Node* tileset_cave_bg() -> engine.open<TilesetNode>()
    .prop<Texture*>(assets.get<Texture>("images/tilesets/cave_bg.png"))
    .exec(tileset_grass_bg_data)
.build();

void cave_bg(NodeBuilder* builder) -> builder.open<TilemapNode>()
    .attach(tileset_cave_bg())
    .prop<float>(1.0f) // scale_x
    .prop<float>(1.0f) // scale_y
    .prop<float>(0.0f) // scroll_offset_x
    .prop<float>(0.0f) // scroll_offset_y
    .prop<float>(0.0f) // scroll_speed_x
    .prop<float>(0.0f) // scroll_speed_y
    .tilemap(0, 0, (lambda(): int -> 1), nullptr)
.close().open<TilemapNode>()
    .attach(tileset_cave_bg())
    .prop<float>(1.0f) // scale_x
    .prop<float>(1.0f) // scale_y
    .prop<float>(0.0f) // scroll_offset_x
    .prop<float>(0.0f) // scroll_offset_y
    .prop<float>(0.1f) // scroll_speed_x
    .prop<float>(0.0f) // scroll_speed_y
    .tilemap(0, 0, (lambda(): int -> 2), nullptr)
.close().open<TilemapNode>()
    .attach(tileset_cave_bg())
    .prop<float>(1.0f) // scale_x
    .prop<float>(1.0f) // scale_y
    .prop<float>(0.0f) // scroll_offset_x
    .prop<float>(0.0f) // scroll_offset_y
    .prop<float>(0.25f) // scroll_speed_x
    .prop<float>(0.0f) // scroll_speed_y
    .tilemap(0, 0, (lambda(): int -> 3), nullptr)
.close().open<TilemapNode>()
    .attach(tileset_cave_bg())
    .prop<float>(1.0f) // scale_x
    .prop<float>(1.0f) // scale_y
    .prop<float>(0.0f) // scroll_offset_x
    .prop<float>(0.0f) // scroll_offset_y
    .prop<float>(0.5f) // scroll_speed_x
    .prop<float>(0.0f) // scroll_speed_y
    .tilemap(0, 0, (lambda(): int -> 4), nullptr)
.close();