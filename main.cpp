#include <iostream>

#include "Allocator.h"

int main() {
    Allocator allocator;
    void *firstBlock = allocator.allocateBlock(10);
    void *secondBlock = allocator.allocateBlock(10);
    return 0;
}
