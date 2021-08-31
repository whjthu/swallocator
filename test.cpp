#include "CAllocator.h"
#include "FreeListAllocator.h"
#include "BlockAllocator.h"
#include <iostream>
#include <vector>
#include <sys/time.h>

// #define LOCAL

#ifdef LOCAL
#include <chrono>
namespace chrono = std::chrono;
#else
// #include "utils.h"
#endif // LOCAL

#define MEMSIZE 2<<28
#include <cstdlib>
#include <iostream>

#ifdef LOCAL
#define HC chrono::high_resolution_clock hc
#define RT(n) auto t##n = hc.now()
#define CT(n,e,s) auto n = chrono::duration_cast<chrono::milliseconds>(t##e - t##s).count()
#else
#define HC 
#define RT(n) auto t##n = get_time()
#define CT(n,e,s) double n = t##e - t##s
#endif // LOCAL

int memtest_seed, memtest_size;

inline double get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
}


// int main() {
//     for (size_t i = 1; i < (1 << 25); i+=i) {
//         double t0 = SWT::get_time();
//         float *p = (float *)malloc(i);
//         double t1 = SWT::get_time();
//         free(p);
//         double t2 = SWT::get_time();
//         std::cout << i << ": " << t1 - t0 << " " << t2 - t1 << std::endl;
//     }   
// }

void testMalloc() {
    HC;
    for (int i = 1; i < 27; ++i) {
        RT(0);
        void *ptr = malloc(2<<i);
        RT(1);
        free(ptr);
        RT(2);
        CT(atime, 1, 0);
        CT(ftime, 2, 1);
        std::cout << "Allocate size: " << (2<<i) << ", time allocate: " << atime << "ms, "
                  << " time free: " << ftime << "ms." << std::endl;
    }
}

void testAllocator(Allocator *allocator) {
    HC;
    for (int i = 1; i < 27; ++i) {
        RT(0);
        void *ptr = allocator->Allocate(2<<i);
        RT(1);
        float *fptr = (float*)ptr;
        for (int j = 0; j < i; j += 2<<(i-1)) {
            fptr[j] = j;
        }
        RT(2);
        allocator->Free(ptr);
        RT(3);
        CT(atime, 1, 0);
        CT(ftime, 3, 2);
        std::cout << "Allocate size: " << (2<<i) << ", time allocate: " << atime << "ms, "
                  << " time free: " << ftime << "ms." << std::endl;
    }
}

void testBA(BlockAllocator *allocator) {
    HC;
    for (int i = 1; i < 27; ++i) {
        RT(0);
        void *ptr = allocator->Allocate(2<<i);
        RT(1);
        float *fptr = (float*)ptr;
        for (int j = 0; j < i; j += 2<<(i-1)) {
            fptr[j] = j;
        }
        RT(2);
        allocator->Free(ptr);
        RT(3);
        CT(atime, 1, 0);
        CT(ftime, 3, 2);
        std::cout << "Allocate size: " << (2<<i) << ", time allocate: " << atime << "ms, "
                  << " time free: " << ftime << "ms." << std::endl;
    }
}


struct MemOp {
    void *ptr;
    size_t size;
    MemOp(void *ptr, size_t size) : ptr(ptr), size(size) {}
};

void testHard(Allocator *allocator) {
    size_t alignment = 1;
    int max_order = 10;
    HC;
    std::vector<MemOp> seq;
    std::vector<int> randseq;
    std::vector<int> randsz;
    size_t szptr = 0;
    srand(memtest_seed);
    for (int i = 0; i < 10000; ++i) {
        randseq.emplace_back(rand() % 2);
    }
    for (int i = 0; i < 100; ++i) {
        randsz.emplace_back(rand() % max_order);
    }
    for (int i = 0; i < 1000; ++i) {
        size_t size = 2 << randsz[szptr++];
        void *ptr = allocator->Allocate(size, alignment);
        szptr %= 100;
        if (ptr != nullptr)
            seq.emplace_back(MemOp(ptr, size));
    }
    RT(0);
    for (int i = 0; i < memtest_size; ++i) {
        if (randseq[i] == 0) {
            size_t size = 2 << randsz[szptr++];
            void* ptr = allocator->Allocate(size, alignment);
            szptr %= 100;
            if (ptr != nullptr)
                seq.emplace_back(MemOp(ptr, size));
        } else {
            if (!seq.empty()) {
                auto idx = randsz[szptr++] % seq.size();
                szptr %= 100;
                allocator->Free(seq[idx].ptr);
                seq.erase(seq.begin() + idx);
            }
        }
    }
    RT(1);
    for (int i = 0; i < seq.size(); ++i) {
        allocator->Free(seq[i].ptr);
    }
    CT(t, 1, 0);
    std::cout << "Time spend: " << t << "ms." << std::endl;
}

void testBAHard(BlockAllocator *allocator) {
    size_t alignment = 1;
    int max_order = 10;
    HC;
    std::vector<MemOp> seq;
    std::vector<int> randseq;
    std::vector<int> randsz;
    size_t szptr = 0;
    srand(memtest_seed);
    for (int i = 0; i < 10000; ++i) {
        randseq.emplace_back(rand() % 2);
    }
    for (int i = 0; i < 100; ++i) {
        randsz.emplace_back(rand() % max_order);
    }
    for (int i = 0; i < 1000; ++i) {
        size_t size = 2 << randsz[szptr++];
        void *ptr = allocator->Allocate(size, alignment);
        szptr %= 100;
        if (ptr != nullptr)
            seq.emplace_back(MemOp(ptr, size));
    }
    RT(0);
    for (int i = 0; i < memtest_size; ++i) {
        if (randseq[i] == 0) {
            size_t size = 2 << randsz[szptr++];
            void* ptr = allocator->Allocate(size, alignment);
            szptr %= 100;
            if (ptr != nullptr)
                seq.emplace_back(MemOp(ptr, size));
        } else {
            if (!seq.empty()) {
                auto idx = randsz[szptr++] % seq.size();
                szptr %= 100;
                allocator->Free(seq[idx].ptr);
                seq.erase(seq.begin() + idx);
            }
        }
    }
    RT(1);
    for (int i = 0; i < seq.size(); ++i) {
        allocator->Free(seq[i].ptr);
    }
    CT(t, 1, 0);
    std::cout << "Time spend: " << t << "ms." << std::endl;
}

int main() {
    char *str_seed = getenv("MEMTEST_SEED");
    memtest_seed = atoi(str_seed);
    char *str_size = getenv("MEMTEST_SIZE");
    memtest_size = atoi(str_size); 
    //testMalloc();
    Allocator *callocator = new CAllocator();
    Allocator *ffallocator = new FreeListAllocator(MEMSIZE, FreeListAllocator::PlacementPolicy::FIND_FIRST);
    Allocator *fballocator = new FreeListAllocator(MEMSIZE, FreeListAllocator::PlacementPolicy::FIND_BEST);
    BlockAllocator *block_allocator = new BlockAllocator(MEMSIZE);

    ffallocator->Init();
    fballocator->Init();

    std::cout << memtest_seed << " " << memtest_size << std::endl;

//     std::cout << "Testing malloc: " << std::endl;
//     testMalloc();
//     std::cout << "Testing CAllocator: " << std::endl;
//     testAllocator(callocator);
//     std::cout << "Testing FreeListAllocator with Find_First: " << std::endl;
//     testAllocator(ffallocator);
//     std::cout << "Testing FreeListAllocator with Find_Best: " << std::endl;
//     testAllocator(fballocator);
//     std::cout << "Testing BlockAllocator: " << std::endl;
//     testBA(block_allocator);
    
    
    std::cout << "Testing CAllocator: " << std::endl;
    testHard(callocator);
    std::cout << "Testing FreeListAllocator with Find_First: " << std::endl;
    testHard(ffallocator);
    std::cout << "Testing FreeListAllocator with Find_Best: " << std::endl;
    testHard(fballocator);
    std::cout << "Testing BlockAllocator: " << std::endl;
    testBAHard(block_allocator);

    return 0;
}
