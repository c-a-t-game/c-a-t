#depends "scripts/engine.c"

Node* level_end() -> engine.open<LevelRootNode>()
    .open<TilemapNode>()
        .open<EntityNode>()
            .event<EntityTextureNode>(lambda level_end_text(): Texture* {
                ui_scaled_label(150, 124, 0xFFFFFFFF, 2, "the end");
                return nullptr;
            })
        .close()
    .close()
.build();
