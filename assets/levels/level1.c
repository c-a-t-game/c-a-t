#include "api/engine.h"

#include "levels/entities/player.h"

static Tile bg[] = {1};
static Tile fg[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

Node* level1() -> engine.open<LevelRootNode>()
    .open<TilemapNode>()
        .open<TilesetNode>()
            .prop<Texture*>(assets.get<Texture>("bg.png"))
            .prop<int>(576) // tile_width
            .prop<int>(384) // tile_height
            .prop<int>(1) // tiles_per_row
            .open<TileNode>().close()
            .open<TileNode>()
                .event<TileTextureNode>(lambda(): int -> 0)
            .close()
        .close()
        .prop<float>(1.0f) // scale_x
        .prop<float>(1.0f) // scale_y
        .prop<float>(-0.125f) // scroll_offset_x
        .prop<float>(-0.125f) // scroll_offset_y
        .prop<float>(0.5f) // scroll_speed_x
        .prop<float>(0.5f) // scroll_speed_y
        .prop<int>(1) // width
        .prop<int>(1) // height
        .prop<Tile>(0) // oob_tile
        .prop<Tile*>(bg) // tiles
    .close()
    .open<TilemapNode>()
        .open<TilesetNode>()
            .prop<Texture*>(assets.get<Texture>("trol.png"))
            .prop<int>(16) // tile_width
            .prop<int>(16) // tile_height
            .prop<int>(16) // tiles_per_row
            .open<TileNode>().close()
            .open<TileNode>()
                .prop<bool>(true) // is_solid
                .event<TileTextureNode>(lambda(): int -> 0)
            .close()
        .close()
        .prop<float>(1.0f) // scale_x
        .prop<float>(1.0f) // scale_y
        .prop<float>(0.0f) // scroll_offset_x
        .prop<float>(0.0f) // scroll_offset_y
        .prop<float>(1.0f) // scroll_speed_x
        .prop<float>(1.0f) // scroll_speed_y
        .prop<int>(24) // width
        .prop<int>(16) // height
        .prop<Tile>(0) // oob_tile
        .prop<Tile*>(fg) // tiles
        .attach(entity_player(1, 2))
    .close()
.build();


