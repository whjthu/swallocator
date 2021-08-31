#include "CAllocator.h"
#include <cstdlib>

CAllocator::CAllocator() : Allocator(0) {}

void CAllocator::Init() {}

CAllocator::~CAllocator() {}

void *CAllocator::Allocate(const size_t size, const size_t alignment) {
    return malloc(size);
}

void CAllocator::Free(void *ptr) {
    free(ptr);
}
