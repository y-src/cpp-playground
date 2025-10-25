//
// Created by Rahul Yadav on 10/03/2025.
//

#include "Allocator.h"

#include <unistd.h>

#include <mutex>

std::mutex globalAllocatorMutex;

BlockHeader_t* Allocator::head{nullptr};
BlockHeader_t* Allocator::tail{nullptr};

Allocator::Allocator(const size_t size) {
    blockSize = size;
    block = allocateBlock(blockSize);
};

Allocator::~Allocator() { freeBlock(block); };

Allocator::Allocator(const Allocator& other) : blockSize{other.blockSize}, block{allocateBlock(other.blockSize)} {
    copyBlock(block, other.block, blockSize);
};

Allocator::Allocator(Allocator&& other) : blockSize{other.blockSize}, block{other.block} {
    other.blockSize = 0;
    other.block = nullptr;
};

Allocator& Allocator::operator=(Allocator&& other) {
    blockSize = other.blockSize;
    block = other.block;
    other.blockSize = 0;
    other.block = nullptr;
    return *this;
};

void Allocator::copyBlock(void* to, void* from, const size_t size) {
    std::lock_guard guard(globalAllocatorMutex);
    char* p = static_cast<char*>(from);
    char* r = static_cast<char*>(to);
    // this is slow, more iterations than using memcpy (which uses long)
    for (int i = 0; i != size; ++i) r[i] = p[i];
};

Allocator& Allocator::operator=(const Allocator& other) {
    // free after, just in case allocation failure
    void* _block = allocateBlock(other.blockSize);
    freeBlock(block);
    blockSize = other.blockSize;
    block = _block;
    // copy the contents
    copyBlock(block, other.block, blockSize);
    return *this;
};

void* Allocator::allocateBlock(const size_t size) {
    if (!size) {
        return nullptr;
    }

    std::lock_guard guard(globalAllocatorMutex);
    BlockHeader_t* blockHeader = getBlock(size);
    // Use the freed block instead of allocating new memory.
    if (blockHeader) {
        blockHeader->isFree = 0;
        return blockHeader + 1;
    }
    const size_t totalSize = size + sizeof(BlockHeader_t);
    // Increase heap size by incrementing `brk`.
    void* block = sbrk(totalSize);
    if (reinterpret_cast<void*>(-1) == block) {
        return nullptr;
    }
    blockHeader = static_cast<BlockHeader_t*>(block);
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
};

BlockHeader_t* Allocator::getBlock(const size_t size) const {
    BlockHeader_t* currentBlock = head;
    // Traverse the linked list of allocated blocks and search for a free block
    // that can fit requested `size`. NOTE: this is a trivial "first-fit"
    // approach.
    while (currentBlock) {
        if (currentBlock->isFree && currentBlock->size >= size) {
            return currentBlock;
        }
        currentBlock = currentBlock->next;
    }
    return nullptr;
};

void Allocator::freeBlock(void* block) {
    if (!block) {
        return;
    }
    std::lock_guard guard(globalAllocatorMutex);
    BlockHeader_t* currentBlock = static_cast<BlockHeader_t*>(block) - 1;
    void* programBreak = sbrk(0);
    // Is the current block allocated before program break?
    if ((currentBlock->size) + static_cast<char*>(block) == programBreak) {
        // Before releasing memory we need to set the tail to the previous
        // block.
        if (head == tail) {
            head = tail = nullptr;
        } else {
            BlockHeader_t* currentPtr = head;
            while (currentPtr) {
                if (currentPtr->next == tail) {
                    currentPtr->next = nullptr;
                    tail = currentPtr;
                }
                currentPtr = currentPtr->next;
            }
        }
        const size_t totalSize = currentBlock->size + sizeof(BlockHeader_t);
        sbrk(-1 * totalSize);
        return;
    }
    // Otherwise mark the block as free.
    currentBlock->isFree = 1;
};
