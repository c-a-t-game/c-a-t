#depends "scripts/decorator.c"

typedef enum {
    Foliage_Tree, Foliage_Bush, Foliage_Count
} FoliageVariants;

Tile foliage_decorator(TilemapNode* tilemap, int x, int y) {
    if (tilemap.get(x, y) != 1) return tilemap.get(x, y);
    if (rand() % 4 != 0) return 1;

    int min = 1, max = 3;
    int variant = rand() % Foliage_Count;
    if (variant == Foliage_Bush) min++, max++;
    int size = rand() % (max - min) + min;
    y--;

    for (int i = 0; i < size; i++) {
        if (x < tilemap.start_x || x >= tilemap.end_x || y < tilemap.start_y || y >= tilemap.end_y) return 1;

        if (tilemap.get(x, y) != 0) return 1;
        if (variant == Foliage_Bush && tilemap.get(x, y + 1) != 1) return 1;

        if (variant == Foliage_Bush) x++;
        if (variant == Foliage_Tree) y--;
    }

    if (variant == Foliage_Bush) {
        x -= size;
        for (int i = 0; i < size; i++)
            tilemap.set(x + i, y, 3);
    }
    if (variant == Foliage_Tree) {
        y += size;
        for (int i = 0; i < size; i++)
            tilemap.set(x, y - i, 2);
    }

    return 1;
}
