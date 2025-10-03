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
        BlockHeader *next;
    };

    BlockAlignment stub;
};

typedef BlockHeader BlockHeader_t;

class Allocator {
public:
    Allocator();

    ~Allocator();

    void *allocateBlock(size_t size);

    BlockHeader_t *head{nullptr};
    BlockHeader_t *tail{nullptr};

private:
    BlockHeader_t *getBlock(size_t size) const;
};

#endif //CPP_PLAYGROUND_ALLOCATOR_H
