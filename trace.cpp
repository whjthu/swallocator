#include "CAllocator.h"
#include "FreeListAllocator.h"
#include "BlockAllocator.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <map>
#include <sys/time.h>

// #define LOCAL

#ifdef LOCAL
#include <chrono>
namespace chrono = std::chrono;
#else
// #include "utils.h"
#endif // LOCAL

#include <cstdlib>
#include <iostream>

constexpr size_t MEMSIZE = ((size_t)100) << 30;

#ifdef LOCAL
#define HC chrono::high_resolution_clock hc
#define RT(n) auto t##n = hc.now()
#define CT(n,e,s) auto n = chrono::duration_cast<chrono::milliseconds>(t##e - t##s).count()
#else
#define HC 
#define RT(n) auto t##n = get_time()
#define CT(n,e,s) double n = t##e - t##s
#endif // LOCAL

int memtest_seed;

inline double get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
}

struct memory_op {
    int type;
    size_t x;
    void *ptr;
    size_t ptr_origin;
    bool flag;
};

size_t cast_addr(const char *str) {
    size_t res;
    if (strlen(str) > 1 && str[0] == '0') {
        sscanf(str, "%lx", &res);
    } else {
        sscanf(str, "%lu", &res);
    }
    return res;
}

class Trace {
  public:
    int n;
    std::vector<memory_op> data;
    std::map<size_t, int> mp;
    Trace() {}
    ~Trace() {}
    void init(const char *filename) {
        std::ifstream fin(filename);
        mp.clear();
        char str[16], str_ptr[64];
        size_t size;
        fin >> n;
        std::cout << n << std::endl; 
        data.resize(n);
        for (int i = 0; i < n; i++) {
            fin >> str;
            if (str[0] == 'a') {
                fin >> size >> str_ptr;
                mp[cast_addr(str_ptr)] = i;
                data[i].type = 0;
                data[i].x = size;
                data[i].ptr = NULL;
                data[i].ptr_origin = cast_addr(str_ptr);
                // std::cout << "[DEBUG] alloc: " << std::hex << data[i].ptr_origin << " " << data[i].x << std::endl;
            } else {
                fin >> str_ptr;
                if (mp.find(cast_addr(str_ptr)) == mp.end() || mp[cast_addr(str_ptr)] == -1) {
                    std::cout << "[ERROR] invalid trace." << std::endl;
                    std::cout << str_ptr << " " << cast_addr(str_ptr) << std::endl;
                    return;
                }
                data[i].type = 1;
                data[i].x = mp[cast_addr(str_ptr)];
                data[i].ptr_origin = cast_addr(str_ptr);
                mp[cast_addr(str_ptr)] = -1;
                // std::cout << "[DEBUG] free: " << data[i].ptr_origin << " " << data[data[i].x].x << std::endl;
            }
        }
    }
};

void simTrace(Allocator *allocator, Trace &trace) {
    HC;
    RT(0);
    std::cout << "[DEBUG] trace.n: " << trace.n << std::endl;
    for (int i = 0; i < trace.n; i++) {
        if (trace.data[i].type == 0) {
            trace.data[i].ptr = allocator->Allocate(trace.data[i].x);
        } else {
            allocator->Free(trace.data[trace.data[i].x].ptr);
        }
    }
    RT(1);
    CT(t, 1, 0);
    std::cout << "Time spend: " << t << "ms." << std::endl;
}

void simBATrace(BlockAllocator *allocator, Trace &trace) {
    HC;
    RT(0);
    // std::cout << "[DEBUG] trace.n: " << trace.n << std::endl;
    size_t used = 0;
    for (int i = 0; i < trace.n; i++) {
        if (trace.data[i].type == 0) {
            trace.data[i].ptr = allocator->Allocate(trace.data[i].x);
            used += trace.data[i].x;
            trace.data[i].flag = true;
            // std::cout << "[DEBUG] alloc: " << trace.data[i].ptr << " " << trace.data[i].x << " " << used << std::endl;
        } else {
            allocator->Free(trace.data[trace.data[i].x].ptr);
            used -= trace.data[trace.data[i].x].x;
            trace.data[trace.data[i].x].flag = false;
            // std::cout << "[DEBUG] free: " << trace.data[trace.data[i].x].ptr << " " << trace.data[trace.data[i].x].x << " " << used << std::endl;
        }
    }

    for (int i = 0; i < trace.n; i++) {
        if (trace.data[i].type == 0 && trace.data[i].flag) {
            // std::cout << "MEM: " << trace.data[i].ptr << " " << trace.data[i].x << " " << std::hex << trace.data[i].ptr_origin << std::endl;
        }

    }
    RT(1);
    CT(t, 1, 0);
    std::cout << "Time spend: " << t << "ms." << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "[ERROR] Trace not found." << std::endl;
    }
    std::cout << "Init trace " << argv[1] << std::endl;

    Trace trace;
    trace.init(argv[1]);
    
    int type = atoi(argv[2]);

    if (type == 0) {
        std::cout << "Simulate CAllocator:" << std::endl;
        Allocator *callocator = new CAllocator();
        simTrace(callocator, trace);
    }

    if (type == 1) {
        Allocator *ffallocator = new FreeListAllocator(MEMSIZE, FreeListAllocator::PlacementPolicy::FIND_FIRST);
        ffallocator->Init();
        std::cout << "Simulate FreeList with First:" << std::endl;
        simTrace(ffallocator, trace);
    }

    if (type == 2) {
        Allocator *fballocator = new FreeListAllocator(MEMSIZE, FreeListAllocator::PlacementPolicy::FIND_BEST);
        fballocator->Init();
        std::cout << "Simulate FreeList with Best:" << std::endl;
        simTrace(fballocator, trace);
    }

    if (type == 3) {
        BlockAllocator *block_allocator = new BlockAllocator(MEMSIZE);
        std::cout << "Simulate BA:" << std::endl;
        simBATrace(block_allocator, trace);
    }

    return 0;
}
