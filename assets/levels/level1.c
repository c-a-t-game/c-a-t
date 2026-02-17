#include "api/engine.h"

#include "levels/entities/player.h"
#include "levels/entities/mouse.h"

static Tile tiles[] = {
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
        .prop<int>(0) // width
        .prop<int>(0) // height
        .prop<Tile>(1) // oob_tile
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
        .prop<Tile*>(tiles) // tiles
        .attach(entity_player(1.5, 14))
        .attach(entity_mouse(22.5, 14))
    .close()
.build();
