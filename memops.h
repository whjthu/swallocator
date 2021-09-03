#pragma once

struct mem_op {
    int type;
    size_t ptr;
    int idx;
    size_t size;
    mem_op() {}
    mem_op(int type_, size_t ptr_, int idx_, size_t size_)
        : type(type_), ptr(ptr_), idx(idx_), size(size_) {}
};

struct mem_trace {
    std::vector<mem_op> ops;
    int size, size_init, cnt;
};

void trace_gen(mem_trace &trace, const std::string &filename);
void trace_load(mem_trace &trace, const std::string &filename);
void trace_dump(const mem_trace &trace, const std::string &filename);
