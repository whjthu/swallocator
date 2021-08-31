#pragma once
#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include <cstddef>

class Allocator {
protected:
    size_t m_totalSize;
    size_t m_used;
    size_t m_peak;

public:
    Allocator(const size_t totalSize) : m_totalSize(totalSize), m_used(0), m_peak(0) {}

    virtual ~Allocator() { m_totalSize = 0; }

    virtual void *Allocate(const size_t size, const size_t alignment = 1) = 0;

    virtual void Free(void *ptr) = 0;

    virtual void Init() = 0;
};

#endif // ALLOCATOR_H
