#include <cstdlib>
#include <iostream>
#include <sys/time.h>
#include <vector>

#include "BlockAllocator.h"
#include "CAllocator.h"
#include "FreeListAllocator.h"
#include "memops.h"

constexpr size_t MEMSIZE = ((size_t)78ul) << 30;

inline double get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
}

void sim_trace(Allocator *allocator, mem_trace &trace) {
    std::vector<void *> addr(trace.cnt);
    double t0 = get_time();
    for (int i = 0; i < trace.size_init; i++) {
        if (trace.ops[i].type == 0) {
            auto &op = trace.ops[i];
            addr[op.idx] = allocator->Allocate(op.size);
        } else {
            auto &op = trace.ops[i];
            allocator->Free(addr[op.idx]);
        }
    }
    double t1 = get_time();
    for (int i = trace.size_init; i < trace.size; i++) {
        if (trace.ops[i].type == 0) {
            auto &op = trace.ops[i];
            addr[op.idx] = allocator->Allocate(op.size);
        } else {
            auto &op = trace.ops[i];
            allocator->Free(addr[op.idx]);
        }
    }
    double t2 = get_time();
    std::cout << "Time init: " << t1 - t0 << " ms" << std::endl;
    std::cout << "Time trace: " << t2 - t1 << " ms" << std::endl;
}

void sim_trace(BlockAllocator *allocator, mem_trace &trace) {
    std::vector<void *> addr(trace.cnt);
    double t0 = get_time();
    for (int i = 0; i < trace.size_init; i++) {
        if (trace.ops[i].type == 0) {
            auto &op = trace.ops[i];
            addr[op.idx] = allocator->Allocate(op.size);
        } else {
            auto &op = trace.ops[i];
            allocator->Free(addr[op.idx]);
        }
    }
    double t1 = get_time();
    for (int i = trace.size_init; i < trace.size; i++) {
        if (trace.ops[i].type == 0) {
            auto &op = trace.ops[i];
            addr[op.idx] = allocator->Allocate(op.size);
        } else {
            auto &op = trace.ops[i];
            allocator->Free(addr[op.idx]);
        }
    }
    double t2 = get_time();
    std::cout << "Time init: " << t1 - t0 << " ms" << std::endl;
    std::cout << "Time trace: " << t2 - t1 << " ms" << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cout << "[ERROR] Invalid args." << std::endl;
        exit(1);
    }
    mem_trace trace;
    if (std::string(argv[1]) == "rand") {
        trace_gen(trace, argv[2]);
    } else if (std::string(argv[1]) == "trace") {
        trace_load(trace, argv[2]);
    } else {
        std::cout << "[ERROR] Invalid args." << std::endl;
        exit(1);
    }
    int type = atoi(argv[3]);

    if (type == 0) {
        std::cout << "CAllocator:" << std::endl;
        Allocator *callocator = new CAllocator();
        sim_trace(callocator, trace);
    }

    if (type == 1) {
        Allocator *ffallocator = new FreeListAllocator(
            MEMSIZE, FreeListAllocator::PlacementPolicy::FIND_FIRST);
        ffallocator->Init();
        std::cout << "FreeList with First:" << std::endl;
        sim_trace(ffallocator, trace);
    }

    if (type == 2) {
        Allocator *fballocator = new FreeListAllocator(
            MEMSIZE, FreeListAllocator::PlacementPolicy::FIND_BEST);
        fballocator->Init();
        std::cout << "FreeList with Best:" << std::endl;
        sim_trace(fballocator, trace);
    }

    if (type == 3) {
        BlockAllocator *block_allocator = new BlockAllocator(MEMSIZE);
        std::cout << "Block Allocator:" << std::endl;
        sim_trace(block_allocator, trace);
    }

    return 0;
}
