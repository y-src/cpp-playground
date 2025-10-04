#include <iostream>

#include "Allocator.h"

int main() {
    Allocator allocator;
    void *firstBlock = allocator.allocateBlock(10);
    void *secondBlock = allocator.allocateBlock(10);
    allocator.freeBlock(firstBlock);
    allocator.freeBlock(secondBlock);
    return 0;
}
