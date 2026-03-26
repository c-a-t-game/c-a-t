#depends "scripts/engine.c"

#depends "scripts/ui.c"

bool editor_play_mode = false;
bool editor_noclip = false;
bool editor_trail = false;
bool editor_picker = false;
bool editor_editing = false;

enum {
    EditorTool_Selection,
    EditorTool_Pencil,
    EditorTool_Eraser,
} editor_tool = EditorTool_Selection;

preserve enum {
    EditorMode_Tilemap,
    EditorMode_Object,
} editor_mode = EditorMode_Tilemap;

bool editor_is_editing() -> engine.editor_mode() && !editor_play_mode;

#define EDITOR_TRAIL_CAPACITY 64

typedef struct {
    float pos_x, pos_y;
    int sprite;
    bool flipped;
} EditorTrail;

typedef Node*(*EntitySpawner)(float x, float y);

EditorTrail editor_trail_data[EDITOR_TRAIL_CAPACITY];
int editor_trail_tail, editor_trail_head, editor_trail_size;
int editor_curr_tile;
EntitySpawner editor_curr_obj;
EntityNode* editor_drag_obj;
LevelRootNode* editor_level;

Node* entity_mouse(float x, float y);
Node* entity_heart(float x, float y);
Node* entity_coin(float x, float y);

EditorTrail* editor_trail_get(int index) -> &editor_trail_data[(editor_trail_tail + index) % EDITOR_TRAIL_CAPACITY];

void editor_push_trail(float pos_x, float pos_y, int sprite, bool flipped) {
    if (!editor_play_mode) return;
    if (editor_trail_size != 0) {
        EditorTrail* prev = editor_trail_get(editor_trail_size - 1);
        float x = pos_x - prev->pos_x;
        float y = pos_y - prev->pos_y;
        if (sqrtf(x * x + y * y) < 0.75) return;
    }
    editor_trail_data[editor_trail_head].pos_x  = pos_x;
    editor_trail_data[editor_trail_head].pos_y  = pos_y;
    editor_trail_data[editor_trail_head].sprite = sprite;
    editor_trail_data[editor_trail_head].flipped = flipped;
    if (editor_trail_size == EDITOR_TRAIL_CAPACITY)
        editor_trail_tail = (editor_trail_tail + 1) % EDITOR_TRAIL_CAPACITY;
    else editor_trail_size++;
    editor_trail_head = (editor_trail_head + 1) % EDITOR_TRAIL_CAPACITY;
}

void editor_play_button() {
    Texture* icons = assets.get<Texture>("images/hud/editor.png");
    ui_container(384 - 25, 256 - 25, 24, 24, 0x3F3F3FFF);
    if (ui_icon_button(384 - 23, 256-23, 20, 20, false, 16 + editor_play_mode * 16, 16, 16, 16, icons) || input.pressed("editor_play")) {
        editor_play_mode ^= 1;
        if (editor_play_mode) {
            __curr_level_node = editor_level.node.copy();
            editor_trail_size = editor_trail_head = editor_trail_tail = 0;
        }
        else {
            LevelRootNode* level = __curr_level_node;
            EntityNode* old_player = level.find("player");
            float old_x = level.cam_x, old_y = level.cam_y;
            old_player.node.detach();
            level.node.delete();
            level = __curr_level_node = editor_level;
            EntityNode* new_player = level.find("player");
            new_player.node.parent.attach(old_player);
            new_player.node.delete();
            level.cam_x = old_x, level.cam_y = old_y;
        }
    }
}

void editor_export() {
    LevelRootNode* level = engine.level();
    EntityNode* player = level.find("player");
    TilemapNode* tilemap = player.node.parent;
    TilesetNode* tileset = tilemap.tileset();

    int width = tilemap.end_x - tilemap.start_x, height = tilemap.end_y - tilemap.start_y;
    printf("#depends \"scripts/entities/player.c\"\n");
    printf("#depends \"scripts/entities/mouse.c\"\n");
    printf("#depends \"scripts/tilesets/grass.c\"\n");
    printf("#depends \"scripts/decorators/foliage.c\"\n");
    printf("\n");
    printf("Node* level() -> engine.open<LevelRootNode>()\n");
    printf("    .exec(grass_bg)\n");
    printf("    .open<TilemapNode>()\n");
    printf("        .attach(tileset_grass())\n");
    printf("        .prop<float>(1.0f) // scale_x\n");
    printf("        .prop<float>(1.0f) // scale_y\n");
    printf("        .prop<float>(%d.0f) // scroll_offset_x\n", tilemap.start_x);
    printf("        .prop<float>(%d.0f) // scroll_offset_y\n", tilemap.start_y);
    printf("        .prop<float>(1.0f) // scroll_speed_x\n");
    printf("        .prop<float>(1.0f) // scroll_speed_y\n");
    printf("        .tilemap(%d, %d, 0, (Tile[]){\n", width, height);
    for (int y = 0; y < height; y++) {
        printf("            ");
        for (int x = 0; x < width; x++) {
            int tile = tilemap.get(x, y);
            printf("%d,", tile);
        }
        printf("\n");
    }
    printf("        })\n");
    for (int i = 0; i < tilemap.node.children_size; i++) {
        if (tilemap.node.children[i] && (int)tilemap.node.children[i].type == NodeType_Entity) {
            EntityNode* entity = tilemap.node.children[i];
            printf("        .attach(%s(%.1ff, %.1ff))\n", entity.func, entity.pos_x, entity.pos_y);
        }
    }
    printf("    .close()\n");
    printf(".build();\n");
}

void editor_init() {
    editor_level = __curr_level_node;
}

void editor_update() {
    Texture* icons = assets.get<Texture>("images/hud/editor.png");
    LevelRootNode* level = engine.level();
    EntityNode* player = level.find("player");
    TilemapNode* tilemap = player.node.parent;
    TilesetNode* tileset = tilemap.tileset();
    float offset_x = level.cam_x * tilemap.scroll_speed_x / tilemap.scale_x / tileset.tile_width  - tilemap.scroll_offset_x;
    float offset_y = level.cam_y * tilemap.scroll_speed_y / tilemap.scale_y / tileset.tile_height - tilemap.scroll_offset_y;
    if (editor_trail) {
        // todo: put this math in src/engine/util.c and make src/engine/render.c use it
        Texture* player_tex = assets.get<Texture>("images/entities/player.png");
        for (int i = 0; i < editor_trail_size; i++) {
            EditorTrail* trail = editor_trail_get(i);
            float tex_w = 16 * (trail.flipped ? -1 : 1);
            float tex_h = 16;
            float x = ((trail.pos_x - offset_x) * tileset.tile_width  - tex_w / 2) * tilemap.scale_x;
            float y = ((trail.pos_y - offset_y) * tileset.tile_height - tex_h)     * tilemap.scale_y;
            float w = tex_w * tilemap.scale_x;
            float h = tex_h * tilemap.scale_y;
            gfx.main().draw(player_tex, x, y, w, h, trail.sprite * 16, 0, 16, 16, 0xFFFFFF7F);
        }
    }

    float sel_x = input.mouse_x() / tilemap.scale_x / tileset.tile_width  + offset_x;
    float sel_y = input.mouse_y() / tilemap.scale_y / tileset.tile_height + offset_y;
    EntityNode* sel_entity = nullptr;
    if (editor_mode == EditorMode_Tilemap) {
        gfx.main().draw(icons,
            (floorf(sel_x) - offset_x) * tileset.tile_width  * tilemap.scale_x,
            (floorf(sel_y) - offset_y) * tileset.tile_height * tilemap.scale_y,
            16, 16, 48, 16, 16, 16, 0xFFFFFFFF
        );
    }
    for (int i = 0; i < tilemap.node.children_size; i++) {
        if (tilemap.node.children[i] && (int)tilemap.node.children[i].type == NodeType_Entity) {
            EntityNode* entity = tilemap.node.children[i];
            float x = entity.pos_x - fmax(entity.width,  0.5f) / 2;
            float y = entity.pos_y - fmax(entity.height, 0.5f);
            if (sel_x >= x && sel_y >= y && sel_x < x + entity.width && sel_y < y + entity.height) sel_entity = entity;
        }
    }

    bool cannot_edit = false;
    cannot_edit |= ui_container(384 - 25, 1+21*0, 24, 3+21*3, 0x3F3F3FFF);
    cannot_edit |= ui_container(384 - 25, 5+21*3, 24, 3+21*2, 0x3F3F3FFF);
    cannot_edit |= ui_container(384 - 25, 9+21*5, 24, 3+21*2, 0x3F3F3FFF);
    cannot_edit |= ui_container(384 - 25, 256 - 25, 24, 24, 0x3F3F3FFF);
    cannot_edit |= ui_container(384 - 25, 256 - 50, 24, 24, 0x3F3F3FFF);
    if (ui_icon_button(384 - 23, 3+21*0, 20, 20, editor_tool == EditorTool_Selection, 0,  0,  16, 16, icons) || input.pressed("editor_select"))  editor_tool = EditorTool_Selection;
    if (ui_icon_button(384 - 23, 3+21*1, 20, 20, editor_tool == EditorTool_Pencil,    16, 0,  16, 16, icons) || input.pressed("editor_pencil"))  editor_tool = EditorTool_Pencil;
    if (ui_icon_button(384 - 23, 3+21*2, 20, 20, editor_tool == EditorTool_Eraser,    32, 0,  16, 16, icons) || input.pressed("editor_eraser"))  editor_tool = EditorTool_Eraser;
    if (ui_icon_button(384 - 23, 7+21*3, 20, 20, editor_mode == EditorMode_Tilemap,   48, 0,  16, 16, icons)) editor_mode = EditorMode_Tilemap;
    if (ui_icon_button(384 - 23, 7+21*4, 20, 20, editor_mode == EditorMode_Object,    0,  16, 16, 16, icons)) editor_mode = EditorMode_Object;
    if (ui_icon_button(384 - 23, 11+21*5, 20, 20, false, 16 - editor_trail  * 16, 32, 16, 16, icons)) editor_trail ^= 1;
    if (ui_icon_button(384 - 23, 11+21*6, 20, 20, false, 48 - editor_noclip * 16, 32, 16, 16, icons)) editor_noclip ^= 1;
    if (ui_icon_button(384 - 23, 256 - 48, 20, 20, false, 0, 48, 16, 16, icons)) editor_export();
    editor_play_button();

    if (input.pressed("editor_mode_toggle")) editor_mode = editor_mode == EditorMode_Tilemap ? EditorMode_Object : EditorMode_Tilemap;

    if (input.pressed("editor_tile_picker")) {
        editor_picker ^= 1;
        if (editor_picker) {
            editor_tool = EditorTool_Pencil;
            editor_mode = EditorMode_Tilemap;
        }
    }
    if (input.pressed("editor_obj_picker")) {
        editor_picker ^= 1;
        if (editor_picker) {
            editor_tool = EditorTool_Pencil;
            editor_mode = EditorMode_Object;
        }
    }
    if (editor_picker) {
        #define OBJECT(name) { \
            assets.get<Texture>("images/entities/" __STR__(name) ".png"), \
            __ID__(entity_, name) \
        }
        struct {
            Texture* tex;
            EntitySpawner spawner;
        } objects[] = { OBJECT(mouse), OBJECT(heart), OBJECT(coin) };

        gfx.main().rect(0, 0, 384, 256, 0x0000007F);
        int tiles[] = { 0, 226, 244, 240, 225, 17, 35 };
        int num_tiles = sizeof(tiles) / sizeof(*tiles);
        int num_objects = sizeof(objects) / sizeof(*objects);
        int num_items = editor_mode == EditorMode_Tilemap ? num_tiles : num_objects;
        int width = 3 + (num_items > 16 ? 16 : num_items) * 21;
        int height = 3 + (num_items / 16 + 1) * 21;
        int panel_x = (384 - width) / 2;
        int panel_y = (256 - height) / 2;
        cannot_edit |= ui_container(panel_x, panel_y, width, height, 0x3F3F3FFF);
        if (editor_mode == EditorMode_Tilemap) for (int tile = 0; tile < num_tiles; tile++) {
            int tex = tiles[tile];
            int tile_x = tile % 16;
            int tile_y = tile / 16;
            int tex_x = tex % 16;
            int tex_y = tex / 16;
            if (ui_icon_button(panel_x + 2+21*tile_x, panel_y + 2+21*tile_y, 20, 20, false, tex_x * 16, tex_y * 16, 16, 16, tileset.tileset)) {
                editor_curr_tile = tile + 1;
                editor_picker = false;
            }
        }
        else if (editor_mode == EditorMode_Object) for (int obj = 0; obj < num_objects; obj++) {
            int obj_x = obj % 16;
            int obj_y = obj / 16;
            if (ui_icon_button(panel_x + 2+21*obj_x, panel_y + 2+21*obj_y, 20, 20, false, 0, 0, 16, 16, objects[obj].tex)) {
                editor_curr_obj = objects[obj].spawner;
                editor_picker = false;
            }
        }
    }

    bool prev_editing = editor_editing;
    if (!cannot_edit) {
        if (input.mouse_pressed(MouseButton_Left)) {
            editor_editing = true;
            if (editor_mode == EditorMode_Object) {
                if (editor_tool == EditorTool_Pencil) if (editor_curr_obj) {
                    editor_drag_obj = editor_curr_obj(sel_x, sel_y);
                    tilemap.node.attach(editor_drag_obj);
                }
                if (editor_tool == EditorTool_Selection) editor_drag_obj = sel_entity;
            }
        }
        if (input.mouse_pressed(MouseButton_Right)) {
            player.pos_x = sel_x;
            player.pos_y = sel_y;
            player.vel_x = player.vel_y = 0;
        }
    }
    if (input.mouse_released(MouseButton_Left)) {
        editor_editing = false;
        editor_drag_obj = nullptr;
    }
    if (editor_editing) {
        if (editor_mode == EditorMode_Tilemap) {
            if (editor_tool == EditorTool_Pencil) {
                if (input.down("ctrl")) editor_curr_tile = tilemap.get(floorf(sel_x), floorf(sel_y));
                else if (input.down("shift")) {} // bucket fill
                else tilemap.set(floorf(sel_x), floorf(sel_y), editor_curr_tile);
            }
            if (editor_tool == EditorTool_Eraser) tilemap.set(floorf(sel_x), floorf(sel_y), 0);
        }
        if (editor_mode == EditorMode_Object) {
            if (editor_tool == EditorTool_Eraser) if (sel_entity) {
                sel_entity.node.delete();
            }
        }
    }
    if (editor_drag_obj) {
        float x = sel_x, y = sel_y;
        y += editor_drag_obj.height / 2;
        if (!(int)input.down("shift")) {
            x = roundf(x * 2) / 2;
            y = roundf(y * 2) / 2;
        }
        editor_drag_obj.pos_x = x;
        editor_drag_obj.pos_y = y;
    }

    Texture* cursor = assets.get<Texture>("images/hud/cursors.png");
    int cursor_icon = 0;
    int origin_x = 1;
    int origin_y = 13;
    if (editor_tool == EditorTool_Pencil) {
        if (editor_mode == EditorMode_Tilemap) {
            if (input.down("ctrl")) cursor_icon = 1;
            else if (input.down("shift")) cursor_icon = 2;
            else cursor_icon = 0;
        }
        else cursor_icon = 0;
    }
    else if (editor_tool == EditorTool_Eraser) cursor_icon = 3;
    else if (editor_tool == EditorTool_Selection) {
        origin_x = 7;
        origin_y = 7;
        if (sel_entity) {
            cursor_icon = editor_editing ? 9 : 8;
            origin_y = 1;
        }
        else if (input.down("shift") && input.down("ctrl")) cursor_icon = 7;
        else if (input.down("shift")) cursor_icon = 5;
        else if (input.down("ctrl")) cursor_icon = 6;
        else cursor_icon = 4;
    }
    gfx.main().draw(cursor, input.mouse_x() - origin_x, input.mouse_y() - origin_y, 14, 14, (cursor_icon % 4) * 14, (cursor_icon / 4) * 14, 14, 14, 0xFFFFFFFF);
}
