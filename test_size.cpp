#include <cstdlib>
#include <iostream>
#include <iostream>
#include <sys/time.h>

#include "BlockAllocator.h"
#include "CAllocator.h"
#include "FreeListAllocator.h"

constexpr size_t MEMSIZE = ((size_t)78ul) << 30;

inline double get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
}

void test(Allocator *allocator) {
    for (int i = 2; i < 28; ++i) {
        for (int j = 0; j < 128; j++) {
            void *ptr = allocator->Allocate(1 << i);
            float *fptr = (float *)ptr;
            for (int j = 0; j < i; j += 1 << (i - 2)) {
                fptr[j] = j;
            }
            allocator->Free(ptr);
        }
        double time_alloc = 0, time_free = 0;
        for (int j = 0; j < 128; j++) {
            double t0 = get_time();
            void *ptr = allocator->Allocate(1 << i);
            double t1 = get_time();
            float *fptr = (float *)ptr;
            for (int j = 0; j < i; j += 1 << (i - 2)) {
                fptr[j] = j;
            }
            double t2 = get_time();
            allocator->Free(ptr);
            double t3 = get_time();
            time_alloc += t1 - t0;
            time_free += t3 - t2;
        }
        std::cout << "size: " << (1 << i) << ", alloc: " << time_alloc / 128
                  << "ms, "
                  << " free: " << time_free / 128 << "ms." << std::endl;
    }
}

void test(BlockAllocator *allocator) {
    for (int i = 2; i < 28; ++i) {
        for (int j = 0; j < 128; j++) {
            void *ptr = allocator->Allocate(1 << i);
            float *fptr = (float *)ptr;
            for (int j = 0; j < i; j += 1 << (i - 2)) {
                fptr[j] = j;
            }
            allocator->Free(ptr);
        }
        double time_alloc = 0, time_free = 0;
        for (int j = 0; j < 128; j++) {
            double t0 = get_time();
            void *ptr = allocator->Allocate(1 << i);
            double t1 = get_time();
            float *fptr = (float *)ptr;
            for (int j = 0; j < i; j += 1 << (i - 2)) {
                fptr[j] = j;
            }
            double t2 = get_time();
            allocator->Free(ptr);
            double t3 = get_time();
            time_alloc += t1 - t0;
            time_free += t3 - t2;
        }
        std::cout << "size: " << (1 << i) << ", alloc: " << time_alloc / 128
                  << "ms, "
                  << " free: " << time_free / 128 << "ms." << std::endl;
    }
}

int main() {
    Allocator *callocator = new CAllocator();
    Allocator *ffallocator = new FreeListAllocator(
        MEMSIZE, FreeListAllocator::PlacementPolicy::FIND_FIRST);
    Allocator *fballocator = new FreeListAllocator(
        MEMSIZE, FreeListAllocator::PlacementPolicy::FIND_BEST);
    BlockAllocator *block_allocator = new BlockAllocator(MEMSIZE);
    ffallocator->Init();
    fballocator->Init();

    std::cout << "Testing CAllocator: " << std::endl;
    test(callocator);
    std::cout << "Testing FreeListAllocator with Find_First: " << std::endl;
    test(ffallocator);
    std::cout << "Testing FreeListAllocator with Find_Best: " << std::endl;
    test(fballocator);
    std::cout << "Testing BlockAllocator: " << std::endl;
    test(block_allocator);
    std::cout << "Testing BlockAllocator without malloc: " << std::endl;
    block_allocator->flag_malloc = false;
    test(block_allocator);

    return 0;
}
