#depends "scripts/engine.c"

bool ui_hovering(int x, int y, int w, int h) -> input.mouse_x() >= x && input.mouse_y() >= y && input.mouse_x() < x + w && input.mouse_y() < y + h;

bool ui_container(int x, int y, int w, int h, int color) {
    Texture* bg = assets.get<Texture>("images/hud/container.png");
    gfx.main().draw(bg, x,         y,         3,     3,     0,  0, 3, 3, 0xFFFFFFFF);
    gfx.main().draw(bg, x + w - 3, y,         3,     3,     4,  0, 3, 3, 0xFFFFFFFF);
    gfx.main().draw(bg, x,         y + h - 3, 3,     3,     0,  4, 3, 3, 0xFFFFFFFF);
    gfx.main().draw(bg, x + w - 3, y + h - 3, 3,     3,     4,  4, 3, 3, 0xFFFFFFFF);
    gfx.main().draw(bg, x + 3,     y,         w - 6, 3,     3,  0, 1, 3, 0xFFFFFFFF);
    gfx.main().draw(bg, x + 3,     y + h - 3, w - 6, 3,     3,  4, 1, 3, 0xFFFFFFFF);
    gfx.main().draw(bg, x,         y + 3,     3,     h - 6, 0,  3, 3, 1, 0xFFFFFFFF);
    gfx.main().draw(bg, x + w - 3, y + 3,     3,     h - 6, 4,  3, 3, 1, 0xFFFFFFFF);
    gfx.main().draw(bg, x + 3,     y + 3,     w - 6, h - 6, 4,  4, 1, 1, 0xFFFFFFFF);
    gfx.main().draw(bg, x,         y,         3,     3,     7,  0, 3, 3, color);
    gfx.main().draw(bg, x + w - 3, y,         3,     3,     11, 0, 3, 3, color);
    gfx.main().draw(bg, x,         y + h - 3, 3,     3,     7,  4, 3, 3, color);
    gfx.main().draw(bg, x + w - 3, y + h - 3, 3,     3,     11, 4, 3, 3, color);
    gfx.main().draw(bg, x + 3,     y,         w - 6, 3,     10, 0, 1, 3, color);
    gfx.main().draw(bg, x + 3,     y + h - 3, w - 6, 3,     10, 4, 1, 3, color);
    gfx.main().draw(bg, x,         y + 3,     3,     h - 6, 7,  3, 3, 1, color);
    gfx.main().draw(bg, x + w - 3, y + 3,     3,     h - 6, 11, 3, 3, 1, color);
    gfx.main().draw(bg, x + 3,     y + 3,     w - 6, h - 6, 10, 3, 1, 1, color);
    return ui_hovering(x, y, w, h);
}

void ui_label(int x, int y, int color, const char* text) {
    Texture* font = assets.get<Texture>("images/hud/font.png");
    char c;
    while ((c = *text++)) {
        int src_x = c % 16;
        int src_y = c / 16 - 2;
        gfx.main().draw(font, x, y, 6, 8, src_x * 6, src_y * 8, 6, 8, color);
        x += 6;
    }
}

bool ui_button(int x, int y, int w, int h, bool disabled, const char* text) {
    bool clicked = false;
    if (disabled) ui_container(x, y, w, h, 0x6F6F6FFF);
    else if (ui_hovering(x, y, w, h)) {
        clicked = input.mouse_released(MouseButton_Left);
        if (input.mouse_down(MouseButton_Left)) ui_container(x, y, w, h, 0x8F8F8FFF);
        else ui_container(x, y, w, h, 0xCFCFCFFF);
    }
    else ui_container(x, y, w, h, 0xAFAFAFFF);
    int text_len = strlen(text) * 6;
    x += (w - text_len) / 2;
    y += (h - 8) / 2;
    ui_label(x, y, 0x1F1F1FFF, text);
    return clicked;
}

bool ui_icon_button(int x, int y, int w, int h, bool disabled, int tex_x, int tex_y, int tex_w, int tex_h, Texture* tex) {
    bool result = ui_button(x, y, w, h, disabled, "");
    x += (w - tex_w) / 2;
    y += (h - tex_h) / 2;
    gfx.main().draw(tex, x, y, tex_w, tex_h, tex_x, tex_y, tex_w, tex_h, 0xFFFFFFFF);
    return result;
}

void ui_hud_number(Texture* tex, int x, int y, int num) {
    if (num >= 100) num = 99;
    int ones = num % 10;
    int tens = num / 10;
    gfx.main().draw(tex, x + 7*0, y, 8, 10, 80, 0, 8, 10, 0xFFFFFFFF);
    gfx.main().draw(tex, x + 7*1, y, 8, 10, 8 * tens, 0, 8, 10, 0xFFFFFFFF);
    gfx.main().draw(tex, x + 7*2, y, 8, 10, 8 * ones, 0, 8, 10, 0xFFFFFFFF);
}

void ui_hud() {
    Texture* icons = assets.get<Texture>("images/hud/icons.png");
    Texture* numbers = assets.get<Texture>("images/hud/numbers.png");
    gfx.main().draw(icons, 4, 4, 12, 12, 0, 0, 12, 12, 0xFFFFFFFF);
    gfx.main().draw(icons, 4, 18, 12, 12, 12, 0, 12, 12, 0xFFFFFFFF);
    ui_hud_number(numbers, 16, 6, *storage.get<int>("num_hearts"));
    ui_hud_number(numbers, 16, 20, *storage.get<int>("num_coins"));
    EntityNode* end = engine.level().find("end");
    if (!end) return;
    int max_coins = *end.prop<int>("max_coins");
    int remaining = *end.prop<int>("num_coins");
    for (int i = 0; i < max_coins; i++) {
        gfx.main().draw(icons, 384 - 4 - 14 * (i + 1), 4, 12, 12, 24 + (i < remaining) * 12, 0, 12, 12, 0xFFFFFFFF);
    }
}
