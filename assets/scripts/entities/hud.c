#depends "scripts/engine.c"

void hud_draw_number(Texture* tex, float x, float y, int num) {
    if (num >= 100) num = 99;
    int ones = num % 10;
    int tens = num / 10;
    gfx.main().draw(tex, x + 7*0, y, 8, 10, 80, 0, 8, 10, 0xFFFFFFFF);
    gfx.main().draw(tex, x + 7*1, y, 8, 10, 8 * tens, 0, 8, 10, 0xFFFFFFFF);
    gfx.main().draw(tex, x + 7*2, y, 8, 10, 8 * ones, 0, 8, 10, 0xFFFFFFFF);
}

Node* entity_hud() -> engine.open<EntityNode>()
    .event<EntityTextureNode>(lambda entity_hud_draw(): Texture* {
        Texture* icons = assets.get<Texture>("images/hud/icons.png");
        Texture* numbers = assets.get<Texture>("images/hud/numbers.png");
        gfx.main().draw(icons, 4, 4, 12, 12, 0, 0, 12, 12, 0xFFFFFFFF);
        gfx.main().draw(icons, 4, 18, 12, 12, 12, 0, 12, 12, 0xFFFFFFFF);
        hud_draw_number(numbers, 16, 6, *storage.get<int>("num_hearts"));
        hud_draw_number(numbers, 16, 20, *storage.get<int>("num_coins"));
        return nullptr;
    })
.build();
