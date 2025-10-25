//``
// Created by Rahul Yadav on 10/03/2025.
//

#ifndef CPP_PLAYGROUND_ALLOCATOR_H
#define CPP_PLAYGROUND_ALLOCATOR_H

#include <cstddef>

typedef char BlockAlignment[16];

union BlockHeader {
    struct {
        size_t size;
        unsigned isFree;
        BlockHeader* next;
    };

    BlockAlignment stub;
};

typedef BlockHeader BlockHeader_t;

class Allocator {
   public:
    explicit Allocator(const size_t size);

    Allocator() : Allocator(0) {};

    ~Allocator();

    Allocator(const Allocator& other);

    Allocator& operator=(const Allocator& other);

    Allocator(Allocator&& other);

    Allocator& operator=(Allocator&& other);

   private:
    size_t blockSize;

    void* block;

    static BlockHeader_t* head;

    static BlockHeader_t* tail;

    void* allocateBlock(const size_t size);

    void freeBlock(void* block);

    BlockHeader_t* getBlock(size_t size) const;

    static void copyBlock(void* to, void* from, const size_t size);
};

#endif  // CPP_PLAYGROUND_ALLOCATOR_H
