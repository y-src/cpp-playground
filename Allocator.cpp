//
// Created by Rahul Yadav on 10/03/2025.
//

#include "Allocator.h"
#include <iostream>
#include <unistd.h>


std::mutex globalAllocatorMutex;


Allocator::Allocator() = default;

Allocator::~Allocator() = default;

void *Allocator::allocateBlock(const size_t size) {
    if (!size) {
        return nullptr;
    }

    std::lock_guard guard(globalAllocatorMutex);
    BlockHeader_t *blockHeader = getBlock(size);
    // Use the freed block instead of allocating new memory.
    if (blockHeader) {
        blockHeader->isFree = 0;
        return blockHeader + 1;
    }
    const size_t totalSize = size + sizeof(BlockHeader_t);
    // Increase heap size by incrementing `brk`.
    void *block = sbrk(totalSize);
    if (reinterpret_cast<void *>(-1) == block) {
        return nullptr;
    }
    blockHeader = static_cast<BlockHeader_t *>(block);
    blockHeader->isFree = 0;
    blockHeader->size = size;
    blockHeader->next = nullptr;
    if (!head) {
        // This block is head.
        head = blockHeader;
    }
    if (tail) {
        // Previously allocated block points to this.
        tail->next = blockHeader;
    }
    // This block is now tail.
    tail = blockHeader;
    return blockHeader + 1;
}

BlockHeader_t *Allocator::getBlock(const size_t size) const {
    BlockHeader_t *currentBlock = head;
    // Traverse the linked list of allocated blocks and search for a free block that can fit requested `size`.
    // NOTE: this is a trivial "first-fit" approach.
    while (currentBlock) {
        if (currentBlock->isFree && currentBlock->size >= size) {
            return currentBlock;
        }
        currentBlock = currentBlock->next;
    }
    return nullptr;
}
