#include "gc.h"
#include <iostream>

int main() {
    GarbageCollector gc(1024 * 1024); // 1MB heap

    void* a = gc.allocate(128);
    void* b = gc.allocate(256);
    void* c = gc.allocate(64);

    std::cout << "Allocated 3 blocks." << std::endl;

    // Simulate a leak by losing references
    a = nullptr;
    b = nullptr;

    gc.collect(); // Should collect unreferenced blocks

    void* d = gc.allocate(512); // Allocate more

    std::cout << "Allocated after GC." << std::endl;

    return 0;
}