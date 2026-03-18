#depends "scripts/editor.c"

#define TILE(x, y) ((y)*16+(x))
#define ANIMATE(frames, delay) ((int)engine.get_millis() % (uint64_t)((frames) * (delay)) / (delay))

Node* tileset_grass() -> engine.open<TilesetNode>()
    .prop<Texture*>(assets.get<Texture>("images/tilesets/grass.png"))
    .prop<int>(16) // tile_width
    .prop<int>(16) // tile_height
    .prop<int>(16) // tiles_per_row
    .open<TileNode>().close() // air
    .open<TileNode>() // ground
        .prop<Collision>(Collision_Solid)
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

            if (tilemap.get(x, y - 1) == 6) {
                if (tilemap.get(x - 1, y - 1) != 6) return TILE(0, 3);
                return TILE(1, 3);
            }
            if (tilemap.get(x, y + 1) == 6) {
                if (tilemap.get(x - 1, y + 1) != 6) return TILE(0, 4);
                return TILE(1, 4);
            }

            int mask = 0;
            if (tilemap.get(x + 0, y - 1) == 1) mask |= N;
            if (tilemap.get(x - 1, y + 0) == 1) mask |= W;
            if (tilemap.get(x + 0, y + 1) == 1) mask |= S;
            if (tilemap.get(x + 1, y + 0) == 1) mask |= E;
            if (tilemap.get(x - 1, y - 1) == 1) mask |= NW;
            if (tilemap.get(x - 1, y + 1) == 1) mask |= SW;
            if (tilemap.get(x + 1, y - 1) == 1) mask |= NE;
            if (tilemap.get(x + 1, y + 1) == 1) mask |= SE;

            if (!(mask & N) || !(mask & W)) mask &= ~NW;
            if (!(mask & S) || !(mask & W)) mask &= ~SW;
            if (!(mask & N) || !(mask & E)) mask &= ~NE;
            if (!(mask & S) || !(mask & E)) mask &= ~SE;

            return mask;
        })
    .close()
    .open<TileNode>() // tree decor
        .event<TileTextureNode>(lambda grass_tree_autotiler(TilemapNode* tilemap, TileNode* tile, int x, int y): int {
            if (tilemap.get(x, y - 1) == 2) return TILE(2, 15);
            return TILE(2, 14);
        })
    .close()
    .open<TileNode>() // bush decor
        .event<TileTextureNode>(lambda grass_bush_autotiler(TilemapNode* tilemap, TileNode* tile, int x, int y): int {
            if (tilemap.get(x - 1, y) != 3) return TILE(3, 15);
            if (tilemap.get(x + 1, y) == 3) return TILE(4, 15);
            return TILE(5, 15);
        })
    .close()
    .open<TileNode>() // crate
        .prop<Collision>(Collision_Solid)
        .event<TileTextureNode>(lambda grass_crate_anim(): int -> (int[]){
            TILE(0, 15), TILE(0, 15), TILE(0, 15), TILE(0, 15),
            TILE(0, 15), TILE(0, 14), TILE(0, 13), TILE(0, 14),
        }[ANIMATE(8, 150)])
    .close()
    .open<TileNode>() // coin
        .event<TileTextureNode>(lambda grass_coin_anim(): int -> (int[]){
            TILE(1, 12), TILE(1, 13), TILE(1, 14), TILE(1, 15),
        }[ANIMATE(4, 150)])
        .event<CollisionNode>(lambda grass_coin_collect(EntityNode* entity, TilemapNode* tilemap, TileNode* tile, int x, int y, Direction direction): void {
            if (editor_is_editing()) return;
            if (!entity.is("player")) return;
            tilemap.set(x, y, 0);
            *storage.get<int>("num_coins") += 1;
        })
    .close()
    .open<TileNode>() // tree stump
        .prop<Collision>(Collision_Solid)
        .event<TileTextureNode>(lambda grass_tree_stump_autotiler(TilemapNode* tilemap, TileNode* tile, int x, int y): int {
            if (tilemap.get(x - 1, y) == 7) return TILE(2, 3);
            if (tilemap.get(x + 1, y) == 7) return TILE(3, 3);
            if (tilemap.get(x - 1, y) == 6) {
                if (tilemap.get(x, y - 1) != 6 && tilemap.get(x, y - 1) != 1) return TILE(1, 1);
                if (tilemap.get(x, y + 1) != 6 && tilemap.get(x, y + 1) != 1) return TILE(1, 5);
                return TILE(1, 2);
            }
            else {
                if (tilemap.get(x, y - 1) != 6 && tilemap.get(x, y - 1) != 1) return TILE(0, 1);
                if (tilemap.get(x, y + 1) != 6 && tilemap.get(x, y + 1) != 1) return TILE(0, 5);
                return TILE(0, 2);
            }
        })
    .close()
    .open<TileNode>() // tree branch
        .prop<Collision>(Collision_TopOnly)
        .event<TileTextureNode>(lambda grass_tree_branch_autotiler(TilemapNode* tilemap, TileNode* tile, int x, int y): int {
            if (tilemap.get(x - 1, y) != 6 && tilemap.get(x - 1, y) != 7) return TILE(2, 2);
            if (tilemap.get(x + 1, y) != 6 && tilemap.get(x + 1, y) != 7) return TILE(4, 2);
            return TILE(3, 2);
        })
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
    .tilemap(0, 0, 1, nullptr)
.close().open<TilemapNode>()
    .attach(tileset_grass_bg())
    .prop<float>(1.0f) // scale_x
    .prop<float>(1.0f) // scale_y
    .prop<float>(0.0f) // scroll_offset_x
    .prop<float>(0.0f) // scroll_offset_y
    .prop<float>(0.1f) // scroll_speed_x
    .prop<float>(0.0f) // scroll_speed_y
    .tilemap(0, 0, 2, nullptr)
.close().open<TilemapNode>()
    .attach(tileset_grass_bg())
    .prop<float>(1.0f) // scale_x
    .prop<float>(1.0f) // scale_y
    .prop<float>(0.0f) // scroll_offset_x
    .prop<float>(0.0f) // scroll_offset_y
    .prop<float>(0.25f) // scroll_speed_x
    .prop<float>(0.0f) // scroll_speed_y
    .tilemap(0, 0, 3, nullptr)
.close().open<TilemapNode>()
    .attach(tileset_grass_bg())
    .prop<float>(1.0f) // scale_x
    .prop<float>(1.0f) // scale_y
    .prop<float>(0.0f) // scroll_offset_x
    .prop<float>(0.0f) // scroll_offset_y
    .prop<float>(0.5f) // scroll_speed_x
    .prop<float>(0.0f) // scroll_speed_y
    .tilemap(0, 0, 4, nullptr)
.close();
