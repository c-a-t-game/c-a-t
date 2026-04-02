#depends "scripts/engine.c"

typedef Tile(*Decorator)(TilemapNode* tilemap, int x, int y);

NodeBuilder* decorate(NodeBuilder* this, Decorator decorator) {
    if (engine.editor_mode()) return this;
    if (!this.curr_node || this.curr_node.children_size == 0) return this;
    TilemapNode* tilemap = this.curr_node.children[this.curr_node.children_size - 1];
    for (int y = tilemap.start_y; y < tilemap.end_y; y++) {
        for (int x = tilemap.start_x; x < tilemap.end_x; x++) {
            tilemap.set(x, y, decorator(tilemap, x, y));
        }
    }
    return this;
}
