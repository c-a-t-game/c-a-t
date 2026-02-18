#depends "levels/level1.c"

void entry_point() {
    input.add("left", 4);
    input.add("right", 7);
    input.add("jump", 44);
    current_level = level1();
}
