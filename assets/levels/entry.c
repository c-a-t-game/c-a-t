#include "engine.h"

extern Node* level1();

void entry_point() {
    input.add("up", 26);
    input.add("down", 22);
    input.add("left", 4);
    input.add("right", 7);
    current_level = level1();
}
