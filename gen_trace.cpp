#include <cstdlib>
#include <iostream>
#include <vector>

struct mem_op {
    void *ptr;
    int type;
    size_t size;
};

void *get_ptr(size_t idx) {
    return reinterpret_cast<void *>(idx);    
}

void gen_trace(std::vector<mem_op> &ops, size_t num_total, size_t num_init, size_t max_size, int memtest_seed) {
    ops.clear();
    std::vector<size_t> cur;
    for (int i = 0; i < num_init; i++) {
        mem_op op;
        cur.emplace_back(i);
        op.ptr = get_ptr(i);
        op.type = 0;
        op.size = rand() % max_size;
        ops.emplace_back(op);
    }

    for (int i = num_init; i < num_total; i++) {
        mem_op op;
        op.type = rand() % 2;
        if (op.type == 0) {
            cur.emplace_back(i);
            op.ptr = get_ptr(i);
            op.type = 0;
            op.size = rand() % max_size;
            ops.emplace_back(op);
        } else {
            size_t idx = rand() % cur.size();
            while (cur[idx] == -1) {
                idx = rand() % cur.size();
            }
            op.ptr = get_ptr(cur[idx]);
            cur[idx] = -1;
            op.type = 1;
            op.size = 0;
            ops.emplace_back(op);
        }
    }

    // size_t alignment = 1;
    // int max_order = 10;
    // HC;
    // std::vector<MemOp> seq;
    // std::vector<int> randseq;
    // std::vector<int> randsz;
    // size_t szptr = 0;
    // srand(memtest_seed);
    // for (int i = 0; i < 10000; ++i) {
    //     randseq.emplace_back(rand() % 2);
    // }
    // for (int i = 0; i < 100; ++i) {
    //     randsz.emplace_back(rand() % max_order);
    // }
    // for (int i = 0; i < 1000; ++i) {
    //     size_t size = 2 << randsz[szptr++];
    //     void *ptr = allocator->Allocate(size, alignment);
    //     szptr %= 100;
    //     if (ptr != nullptr)
    //         seq.emplace_back(MemOp(ptr, size));
    // }
    // RT(0);
    // for (int i = 0; i < memtest_size; ++i) {
    //     if (randseq[i] == 0) {
    //         size_t size = 2 << randsz[szptr++];
    //         void* ptr = allocator->Allocate(size, alignment);
    //         szptr %= 100;
    //         if (ptr != nullptr)
    //             seq.emplace_back(MemOp(ptr, size));
    //     } else {
    //         if (!seq.empty()) {
    //             auto idx = randsz[szptr++] % seq.size();
    //             szptr %= 100;
    //             allocator->Free(seq[idx].ptr);
    //             seq.erase(seq.begin() + idx);
    //         }
    //     }
    // }
    // RT(1);
    // for (int i = 0; i < seq.size(); ++i) {
    //     allocator->Free(seq[i].ptr);
    // }
    // CT(t, 1, 0);
    // std::cout << "Time spend: " << t << "ms." << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        std::cout << "[ERROR] invalid args." << std::endl;
        return 0;
    }
    size_t num_total = atoi(argv[1]);
    size_t num_init = atoi(argv[2]);
    size_t max_size = atoi(argv[3]);
    int memtest_seed = atoi(argv[4]);
    std::vector<mem_op> ops;
    gen_trace(ops, num_total, num_init, max_size, memtest_seed);
    std::cout << ops.size() << std::endl;
    for (auto &op : ops) {
        if (op.type == 0) {
            std::cout << "alloc " << op.size << " " << op.ptr << std::endl;
        } else {
            std::cout << "free " << op.ptr << std::endl;
        }
    }
    return 0;
}
