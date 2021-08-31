#ifndef CALLOCATOR_H
#define CALLOCATOR_H

#include "Allocator.h"

class CAllocator : public Allocator {
public:
    CAllocator();

    virtual ~CAllocator();

    virtual void *Allocate(const size_t size, const size_t aligment = 0) override;

    virtual void Free(void *ptr) override;

    virtual void Init() override;
};

#endif // CALLOCATOR_H
