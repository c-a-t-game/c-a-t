NODE(LevelRoot,
    _(float, cam_x, _)
    _(float, cam_y, _)
)

NODE(TileChunk,
    _(int, x, raw)
    _(int, y, raw)
    _(uint8_t[CHUNK_SIZE][CHUNK_SIZE], tile, raw)
)

NODE(Tilemap,
    _(gpc_polygon, collision, _)
    _(float, scale_x, raw)
    _(float, scale_y, raw)
    _(float, scroll_offset_x, raw)
    _(float, scroll_offset_y, raw)
    _(float, scroll_speed_x, raw)
    _(float, scroll_speed_y, raw)
)

NODE(Tileset,
    _(Texture*, tileset, asset)
    _(int, tile_width, raw)
    _(int, tile_height, raw)
    _(int, tiles_per_row, raw)
)

NODE(Tile,
    _(gpc_polygon, collision, polygon)
)

NODE(Entity,
    _(float, pos_x, raw)
    _(float, pos_y, raw)
    _(float, vel_x, _)
    _(float, vel_y, _)
    _(float, width, raw)
    _(float, height, raw)
)

NODE(CustomRender,
    _(void(*)(), func, asset)
)

NODE(TileTexture,
    _(int(*)(TilemapNode* tilemap, TileNode* tile, int x, int y), func, asset)
)

NODE(TileTextureCollision,
    _(void(*)(EntityNode* entity, TilemapNode* tilemap, TileNode* tile, int x, int y, Direction direction), func, asset)
)

NODE(EntityUpdate,
    _(void(*)(EntityNode* entity, TilemapNode* tilemap), func, asset)
)

NODE(EntityTexture,
    _(Texture*(*)(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h), func, asset)
)

NODE(EntityCollision,
    _(void(*)(EntityNode* entity, TilemapNode* tilemap, TileNode* tile, int x, int y, Direction direction), func, asset)
)
