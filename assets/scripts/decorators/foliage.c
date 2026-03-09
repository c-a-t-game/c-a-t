#depends "scripts/decorator.c"

typedef enum {
    Foliage_Tree, Foliage_Bush, Foliage_Count
} FoliageVariants;

Tile foliage_decorator(TilemapNode* tilemap, int x, int y) {
    if (*tilemap.tile(x, y) != 1) return *tilemap.tile(x, y);
    if (rand() % 4 != 0) return 1;

    int min = 1, max = 3;
    int variant = rand() % Foliage_Count;
    if (variant == Foliage_Bush) min++, max++;
    int size = rand() % (max - min) + min;
    y--;

    for (int i = 0; i < size; i++) {
        if (*tilemap.tile(x, y) != 0) return 1;
        if (variant == Foliage_Bush && *tilemap.tile(x, y + 1) != 1) return 1;

        if (variant == Foliage_Bush) x++;
        if (variant == Foliage_Tree) y--;
    }

    if (variant == Foliage_Bush) {
        x -= size;
        for (int i = 0; i < size; i++)
            *tilemap.tile(x + i, y) = 3;
    }
    if (variant == Foliage_Tree) {
        y += size;
        for (int i = 0; i < size; i++)
            *tilemap.tile(x, y - i) = 2;
    }

    return 1;
}
