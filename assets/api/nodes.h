NODE(LevelRoot,
    float cam_x, cam_y;
)

NODE(Tilemap,
    float scale_x, scale_y;
    float scroll_offset_x, scroll_offset_y;
    float scroll_speed_x, scroll_speed_y;
    int width, height;
    uint8_t oob_tile, *tiles;
)

NODE(Tileset,
    Texture* tileset;
    int tile_width, tile_height;
    int tiles_per_row;
)

NODE(Tile,
    bool is_solid;
)

NODE(Entity,
    float pos_x, pos_y;
    float vel_x, vel_y;
    float width, height;
    struct {
        int count, capacity;
        struct {
            char* key;
            void* value;
        }* entries;
    } data;
)

NODE(CustomRender,
    void(*func)();
)

NODE(TileTexture,
    int(*func)(TilemapNode* tilemap, TileNode* tile, int x, int y);
)

NODE(EntityUpdate,
    void(*func)(EntityNode* entity, TilemapNode* tilemap, float delta_time);
)

NODE(EntityTexture,
    Texture*(*func)(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h);
)

NODE(Collision,
    void(*func)(EntityNode* entity, TilemapNode* tilemap, TileNode* tile, int x, int y, Direction direction);
)
