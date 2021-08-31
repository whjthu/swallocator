#ifndef BUDDYALLOCATOR_H
#define BUDDYALLOCATOR_H

#include "Allocator.h"
#include "DoublyLinkedList.h"
#include <cstdint>

class BuddyAllocator : public Allocator {
public:
    enum PlacementPolicy {
        FIND_FIRST,
        FIND_BEST,
    };

private:
    struct FreeHeader {
        size_t blockSize;
    };
    struct AllocationHeader {
        size_t blockSize;
        uint32_t padding;
    };

    using Node = DoublyLinkedList<FreeHeader>::Node;

    void *m_start_ptr = nullptr;
    PlacementPolicy m_pPolicy;
    DoublyLinkedList<FreeHeader> m_freeList;

public:
    BuddyAllocator(const size_t totalSize, const PlacementPolicy pPolicy);

    virtual ~BuddyAllocator();

    virtual void *Allocate(const size_t size, const size_t alignment = 1) override;

    virtual void Free(void *ptr) override;

    virtual void Init() override;
    
    virtual void Reset();

private:
    BuddyAllocator(BuddyAllocator &freeListAllocator);

    void Coalescence(Node *prevBlock, Node *freeBlock);

    void Find(const size_t sizem, const size_t aligment, size_t &padding, Node *&previousNode, Node *&foundNode);

    void FindBest(const size_t size, const size_t aligment, size_t &padding, Node *&previousNode, Node *&foundNode);
    
    void FindFirst(const size_t size, const size_t aligment, size_t &padding, Node *&previousNode, Node *&foundNode);
};

#endif // BUDDYALLOCATOR_H
