Node* tileset_grass() -> engine.open<TilesetNode>()
    .prop<Texture*>(assets.get<Texture>("images/tilesets/grass.png"))
    .prop<int>(16) // tile_width
    .prop<int>(16) // tile_height
    .prop<int>(16) // tiles_per_row
    .open<TileNode>().close() // air
    .open<TileNode>() // ground
        .prop<bool>(true) // solid
        .event<TileTextureNode>(lambda grass_autotiler(TilemapNode* tilemap, TileNode* tile, int x, int y): int {
            enum {
                N = (1 << 0),
                W = (1 << 1),
                S = (1 << 2),
                E = (1 << 3),
                NW = (1 << 4),
                SW = (1 << 5),
                NE = (1 << 6),
                SE = (1 << 7),
            };

            int mask = 0;
            if (*tilemap.tile(x + 0, y - 1) == 1) mask |= N;
            if (*tilemap.tile(x - 1, y + 0) == 1) mask |= W;
            if (*tilemap.tile(x + 0, y + 1) == 1) mask |= S;
            if (*tilemap.tile(x + 1, y + 0) == 1) mask |= E;
            if (*tilemap.tile(x - 1, y - 1) == 1) mask |= NW;
            if (*tilemap.tile(x - 1, y + 1) == 1) mask |= SW;
            if (*tilemap.tile(x + 1, y - 1) == 1) mask |= NE;
            if (*tilemap.tile(x + 1, y + 1) == 1) mask |= SE;
        
            if (!(mask & N) || !(mask & W)) mask &= ~NW;
            if (!(mask & S) || !(mask & W)) mask &= ~SW;
            if (!(mask & N) || !(mask & E)) mask &= ~NE;
            if (!(mask & S) || !(mask & E)) mask &= ~SE;
            
            return mask;
        })
    .close()
    .open<TileNode>() // branch
        .event<TileTextureNode>(lambda(): int -> 242)
    .close()
    .open<TileNode>() // tree
        .event<TileTextureNode>(lambda(): int -> 226)
    .close()
    .open<TileNode>() // bush left
        .event<TileTextureNode>(lambda(): int -> 243)
    .close()
    .open<TileNode>() // bush middle
        .event<TileTextureNode>(lambda(): int -> 244)
    .close()
    .open<TileNode>() // bush right
        .event<TileTextureNode>(lambda(): int -> 244)
    .close()
.build();

Node* tileset_grass_bg() -> engine.open<TilesetNode>()
    .prop<Texture*>(assets.get<Texture>("images/tilesets/grass_bg.png"))
    .prop<int>(512) // tile_width
    .prop<int>(256) // tile_height
    .prop<int>(2) // tiles_per_row
    .open<TileNode>().close()
    .open<TileNode>()
        .event<TileTextureNode>(lambda(): int -> 0)
    .close()
    .open<TileNode>()
        .event<TileTextureNode>(lambda(): int -> 1)
    .close()
    .open<TileNode>()
        .event<TileTextureNode>(lambda(): int -> 2)
    .close()
    .open<TileNode>()
        .event<TileTextureNode>(lambda(): int -> 3)
    .close()
.build();

void grass_bg(NodeBuilder* builder) -> builder.open<TilemapNode>()
    .attach(tileset_grass_bg())
    .prop<float>(1.0f) // scale_x
    .prop<float>(1.0f) // scale_y
    .prop<float>(0.0f) // scroll_offset_x
    .prop<float>(0.0f) // scroll_offset_y
    .prop<float>(0.0f) // scroll_speed_x
    .prop<float>(0.0f) // scroll_speed_y
    .prop<int>(0) // width
    .prop<int>(0) // height
    .prop<Tile>(1) // oob_tile
.close().open<TilemapNode>()
    .attach(tileset_grass_bg())
    .prop<float>(1.0f) // scale_x
    .prop<float>(1.0f) // scale_y
    .prop<float>(0.0f) // scroll_offset_x
    .prop<float>(0.0f) // scroll_offset_y
    .prop<float>(0.1f) // scroll_speed_x
    .prop<float>(0.1f) // scroll_speed_y
    .prop<int>(0) // width
    .prop<int>(0) // height
    .prop<Tile>(2) // oob_tile
.close().open<TilemapNode>()
    .attach(tileset_grass_bg())
    .prop<float>(1.0f) // scale_x
    .prop<float>(1.0f) // scale_y
    .prop<float>(0.0f) // scroll_offset_x
    .prop<float>(0.0f) // scroll_offset_y
    .prop<float>(0.25f) // scroll_speed_x
    .prop<float>(0.25f) // scroll_speed_y
    .prop<int>(0) // width
    .prop<int>(0) // height
    .prop<Tile>(3) // oob_tile
.close().open<TilemapNode>()
    .attach(tileset_grass_bg())
    .prop<float>(1.0f) // scale_x
    .prop<float>(1.0f) // scale_y
    .prop<float>(0.0f) // scroll_offset_x
    .prop<float>(0.0f) // scroll_offset_y
    .prop<float>(0.5f) // scroll_speed_x
    .prop<float>(0.5f) // scroll_speed_y
    .prop<int>(0) // width
    .prop<int>(0) // height
    .prop<Tile>(4) // oob_tile
.close();
