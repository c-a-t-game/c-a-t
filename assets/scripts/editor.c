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

enum {
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

EditorTrail editor_trail_data[EDITOR_TRAIL_CAPACITY];
int editor_trail_tail, editor_trail_head, editor_trail_size;
int editor_curr_tile;
LevelRootNode* editor_level;

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
    if (ui_icon_button(384 - 23, 256-23, 20, 20, false, 16 + editor_play_mode * 16, 16, 16, 16, icons)) {
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
    if (editor_mode == EditorMode_Tilemap) {
        gfx.main().draw(icons,
            (floorf(sel_x) - offset_x) * tileset.tile_width  * tilemap.scale_x,
            (floorf(sel_y) - offset_y) * tileset.tile_height * tilemap.scale_y,
            16, 16, 48, 16, 16, 16, 0xFFFFFFFF
        );
    }

    bool cannot_edit = false;
    cannot_edit |= ui_container(384 - 25, 1+21*0, 24, 3+21*3, 0x3F3F3FFF);
    cannot_edit |= ui_container(384 - 25, 5+21*3, 24, 3+21*2, 0x3F3F3FFF);
    cannot_edit |= ui_container(384 - 25, 9+21*5, 24, 3+21*2, 0x3F3F3FFF);
    cannot_edit |= ui_container(384 - 25, 256 - 25, 24, 24, 0x3F3F3FFF);
    if (ui_icon_button(384 - 23, 3+21*0, 20, 20, editor_tool == EditorTool_Selection, 0,  0,  16, 16, icons) || input.pressed("editor_select"))  editor_tool = EditorTool_Selection;
    if (ui_icon_button(384 - 23, 3+21*1, 20, 20, editor_tool == EditorTool_Pencil,    16, 0,  16, 16, icons) || input.pressed("editor_pencil"))  editor_tool = EditorTool_Pencil;
    if (ui_icon_button(384 - 23, 3+21*2, 20, 20, editor_tool == EditorTool_Eraser,    32, 0,  16, 16, icons) || input.pressed("editor_eraser"))  editor_tool = EditorTool_Eraser;
    if (ui_icon_button(384 - 23, 7+21*3, 20, 20, editor_mode == EditorMode_Tilemap,   48, 0,  16, 16, icons) || input.pressed("editor_tilemap")) editor_mode = EditorMode_Tilemap;
    if (ui_icon_button(384 - 23, 7+21*4, 20, 20, editor_mode == EditorMode_Object,    0,  16, 16, 16, icons) || input.pressed("editor_object"))  editor_mode = EditorMode_Object;
    if (ui_icon_button(384 - 23, 11+21*5, 20, 20, false, 16 - editor_trail  * 16, 32, 16, 16, icons)) editor_trail ^= 1;
    if (ui_icon_button(384 - 23, 11+21*6, 20, 20, false, 48 - editor_noclip * 16, 32, 16, 16, icons)) editor_noclip ^= 1;
    editor_play_button();

    if (input.pressed("editor_picker")) editor_picker ^= 1;
    if (editor_picker) {
        gfx.main().rect(0, 0, 384, 256, 0x0000007F);
        int tiles[] = { 0, 226, 244, 240, 225, 17, 35 };
        int num_tiles = sizeof(tiles) / sizeof(int);
        int width = 3 + (num_tiles > 16 ? 16 : num_tiles) * 21;
        int height = 3 + (num_tiles / 16 + 1) * 21;
        int panel_x = (384 - width) / 2;
        int panel_y = (256 - height) / 2;
        cannot_edit |= ui_container(panel_x, panel_y, width, height, 0x3F3F3FFF);
        for (int tile = 0; tile < num_tiles; tile++) {
            int tex = tiles[tile];
            int tile_x = tile % 16;
            int tile_y = tile / 16;
            int tex_x = tex % 16;
            int tex_y = tex / 16;
            if (ui_icon_button(panel_x + 2+21*tile_x, panel_y + 2+21*tile_y, 20, 20, false, tex_x * 16, tex_y * 16, 16, 16, tileset.tileset)) {
                editor_curr_tile = tile + 1;
                editor_tool = EditorTool_Pencil;
                editor_mode = EditorMode_Tilemap;
                editor_picker = false;
            }
        }
    }

    bool prev_editing = editor_editing;
    if (!cannot_edit) {
        if (input.mouse_pressed(MouseButton_Left))  editor_editing = true;
        if (input.mouse_pressed(MouseButton_Right)) {
            player.pos_x = sel_x;
            player.pos_y = sel_y;
            player.vel_x = player.vel_y = 0;
        }
    }
    if (input.mouse_released(MouseButton_Left)) editor_editing = false;
    if (editor_editing) {
        if (editor_mode == EditorMode_Tilemap) {
            if (editor_tool == EditorTool_Pencil) {
                if (input.down("shift")) editor_curr_tile = tilemap.get(floorf(sel_x), floorf(sel_y));
                else if (input.down("ctrl")) {} // bucket fill
                else tilemap.set(floorf(sel_x), floorf(sel_y), editor_curr_tile);
            }
            if (editor_tool == EditorTool_Eraser) tilemap.set(floorf(sel_x), floorf(sel_y), 0);
        }
    }

    Texture* cursor = assets.get<Texture>("images/hud/cursors.png");
    int cursor_icon = 0;
    if (editor_tool == EditorTool_Pencil) {
        if (input.down("shift")) cursor_icon = 1;
        else if (input.down("ctrl")) cursor_icon = 2;
        else cursor_icon = 0;
    }
    else if (editor_tool == EditorTool_Eraser) cursor_icon = 3;
    else {
        if (input.down("shift") && input.down("ctrl")) cursor_icon = 7;
        else if (input.down("shift")) cursor_icon = 5;
        else if (input.down("ctrl")) cursor_icon = 6;
        else cursor_icon = 4;
    }
    gfx.main().draw(cursor, input.mouse_x() - 1, input.mouse_y() - 13, 14, 14, (cursor_icon % 4) * 14, (cursor_icon / 4) * 14, 14, 14, 0xFFFFFFFF);
}
