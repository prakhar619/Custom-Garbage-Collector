#ifndef GC_H
#define GC_H

#include <cstddef>
#include <vector>

struct BlockHeader {
    size_t size;
    bool marked;
    bool free;
    BlockHeader* next;
};

class GarbageCollector {
public:
    GarbageCollector(size_t heapSize);
    ~GarbageCollector();

    void* allocate(size_t size);
    void collect();

private:
    BlockHeader* heapStart;
    BlockHeader* freeList;
    void* stackBottom;

    void mark();
    void sweep();
    void coalesce();
    void markFromRoot(void* root);
    bool isPointer(void* p);
};

#endif // GC_H