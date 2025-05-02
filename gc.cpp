#include "gc.h"
#include <unistd.h>
#include <cstring>
#include <iostream>

GarbageCollector::GarbageCollector(size_t heapSize) {
    heapStart = (BlockHeader*)sbrk(heapSize);
    heapStart->size = heapSize - sizeof(BlockHeader);
    heapStart->marked = false;
    heapStart->free = true;
    heapStart->next = nullptr;
    freeList = heapStart;

    // Capturing stack bottom
    int stackDummy;
    stackBottom = &stackDummy;
}

GarbageCollector::~GarbageCollector() {
    // Manual memory management
}

void* GarbageCollector::allocate(size_t size) {
    BlockHeader* current = freeList;

    while (current) {
        if (current->free && current->size >= size) {
            size_t remaining = current->size - size;
            if (remaining > sizeof(BlockHeader) + 8) {
                BlockHeader* newBlock = (BlockHeader*)((char*)current + sizeof(BlockHeader) + size);
                newBlock->size = remaining - sizeof(BlockHeader);
                newBlock->marked = false;
                newBlock->free = true;
                newBlock->next = current->next;
                current->next = newBlock;
                current->size = size;
            }

            current->free = false;
            return (char*)current + sizeof(BlockHeader);
        }
        current = current->next;
    }

    collect(); // Try GC if allocation fails
    return allocate(size); // Retry after GC
}

void GarbageCollector::collect() {
    mark();
    sweep();
    coalesce();
}

void GarbageCollector::mark() {
    int dummy;
    void* stackTop = &dummy;

    char* p = (char*)stackTop;
    while (p < (char*)stackBottom) {
        void* possiblePtr = *(void**)p;
        markFromRoot(possiblePtr);
        p += sizeof(void*);
    }
}

void GarbageCollector::markFromRoot(void* ptr) {
    BlockHeader* current = heapStart;
    while (current) {
        void* blockPtr = (char*)current + sizeof(BlockHeader);
        if (ptr >= blockPtr && ptr < (char*)blockPtr + current->size) {
            if (!current->marked && !current->free) {
                current->marked = true;
            }
        }
        current = current->next;
    }
}

void GarbageCollector::sweep() {
    BlockHeader* current = heapStart;
    while (current) {
        if (!current->marked && !current->free) {
            current->free = true;
        }
        current->marked = false;
        current = current->next;
    }
}

void GarbageCollector::coalesce() {
    BlockHeader* current = heapStart;
    while (current && current->next) {
        if (current->free && current->next->free) {
            current->size += sizeof(BlockHeader) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}