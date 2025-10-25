#include <utility>

#include "Allocator.h"

int main() {
    Allocator allocator1(10);
    Allocator allocator2(100);

    // copy
    Allocator allocator3;
    allocator3 = allocator2;

    // move
    Allocator allocator4;
    allocator4 = std::move(allocator3);

    return 0;
};