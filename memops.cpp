#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "memops.h"

void trace_gen(mem_trace &trace, const std::string &filename) {
    size_t max_size;
    int rand_seed;
    std::ifstream fin(filename);
    fin >> trace.size >> trace.size_init >> max_size >> rand_seed;
    srand(rand_seed + 1);
    trace.ops.clear();
    std::vector<size_t> cur;
    int cnt = 0;
    for (int i = 0; i < trace.size_init; i++) {
        cur.emplace_back(cnt);
        trace.ops.emplace_back(0, cnt, cnt, rand() % max_size + 1);
        cnt++;
    }
    for (int i = trace.size_init; i < trace.size; i++) {
        if (rand() % 2 == 0) {
            cur.emplace_back(cnt);
            trace.ops.emplace_back(0, cnt, cnt, rand() % max_size + 1);
            cnt++;
        } else {
            size_t idx = rand() % cur.size();
            while (cur[idx] == -1) {
                idx = rand() % cur.size();
            }
            trace.ops.emplace_back(1, cur[idx], cur[idx], 0);
            cur[idx] = -1;
        }
    }
    trace.cnt = cnt;
}

void trace_load(mem_trace &trace, const std::string &filename) {
    std::ifstream fin(filename);
    int size, size_init;
    fin >> size >> size_init;
    trace.size = size;
    trace.size_init = size_init;
    if (size_init > size) {
        std::cout << "[ERROR] Invalid trace." << std::endl;
        exit(1);
    }
    trace.ops.clear();
    std::unordered_map<size_t, int> mp;
    mp.clear();
    int cnt = 0;
    size_t alloc_size, ptr;
    std::string str_type, str_ptr;
    for (int i = 0; i < size; i++) {
        fin >> str_type;
        if (str_type == "alloc") {
            fin >> alloc_size >> str_ptr;
            sscanf(str_ptr.c_str(), "%lx", &ptr);
            if (mp.find(ptr) != mp.end()) {
                // std::cout << "[ERROR] Invalia trace: malloc." << std::endl;
                if (i < size_init) {
                    trace.size_init--;
                }
                trace.size--;
                continue;
            }
            mp.emplace(ptr, cnt);
            trace.ops.emplace_back(0, ptr, cnt, alloc_size);
            cnt++;
            continue;
        }
        if (str_type == "free") {
            fin >> str_ptr;
            sscanf(str_ptr.c_str(), "%lx", &ptr);
            if (mp.find(ptr) == mp.end()) {
                // std::cout << "[ERROR] Invalia trace: free." << std::endl;
                if (i < size_init) {
                    trace.size_init--;
                }
                trace.size--;
                continue;
            }
            trace.ops.emplace_back(1, ptr, mp[ptr], 0);
            mp.erase(ptr);
        }
    }
    trace.cnt = cnt;
}

void trace_dump(const mem_trace &trace, const std::string &filename) {
    std::ofstream fout(filename);
    fout << trace.size << " " << trace.size_init << std::endl;
    for (auto &op : trace.ops) {
        if (op.type == 0) {
            fout << "alloc " << op.size << " " << std::hex << op.ptr << std::dec
                 << std::endl;
        } else {
            fout << "free " << std::hex << op.ptr << std::dec << std::endl;
        }
    }
}
