#depends "scripts/engine.c"

typedef Tile(*Decorator)(TilemapNode* tilemap, int x, int y);

NodeBuilder* decorate(NodeBuilder* this, Decorator decorator) {
    if (!this.curr_node || this.curr_node.children_size == 0) return this;
    TilemapNode* tilemap = this.curr_node.children[this.curr_node.children_size - 1];
    for (int y = 0; y < tilemap.height; y++) {
        for (int x = 0; x < tilemap.width; x++) {
            *tilemap.tile(x, y) = decorator(tilemap, x, y);
        }
    }
    return this;
}
