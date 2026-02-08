#include "api/engine.h"

extern Node* level1();

void entry_point() {
    input.add("left", 4);
    input.add("right", 7);
    input.add("jump", 44);
    current_level = level1();
}
