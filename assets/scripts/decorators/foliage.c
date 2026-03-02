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
        if (variant == Foliage_Bush) x++;
        if (variant == Foliage_Tree) y--;
    }
    
    if (variant == Foliage_Bush) {
        x -= size;
        *tilemap.tile(x, y) = 4;
        for (int i = 1; i < size - 1; i++)
            *tilemap.tile(x + i, y) = 5;
        *tilemap.tile(x + size - 1, y) = 6;
    }
    if (variant == Foliage_Tree) {
        y += size;
        for (int i = 0; i < size - 1; i++)
            *tilemap.tile(x, y - i) = 2;
        *tilemap.tile(x, y - size + 1) = 3;
    }
    
    return 1;
}