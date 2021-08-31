#pragma once
#ifndef BLOCKALLOCATOR_H
#define BLOCKALLOCATOR_H

#include "DoublyLinkedList.h"
// #include "PoolAllocator.h"
#include <cstdint>
#include <set>
#include <map>

#define MAX_MEM_SIZE (1ul<<35)
#define DEFAULT_ALIGNMENT 256
//#define DEBUG_ALLOCATOR
#define SMALL_ALLOCATION_SIZE 131072 // 128KB

class BlockAllocator {
public:
    enum PlacementPolicy {
    //    FIND_FIRST,
        FIND_BEST,
    };

    //used for FIND_BEST
    struct FreeNodeInfo {
        size_t addr;
        size_t blockSize;
        FreeNodeInfo(size_t _addr, size_t _blockSize):addr(_addr), blockSize(_blockSize) {}
    };
    struct CMPFreeNodeInfo {
        bool operator() (const FreeNodeInfo& a, const FreeNodeInfo& b) const
        {
            return (a.blockSize != b.blockSize) ? (a.blockSize < b.blockSize)
                                                : (a.addr < b.addr);
        }
    };
    size_t m_totalSize;
    size_t m_used;
    #ifdef DEBUG_ALLOCATOR
    size_t m_asked;
    #endif
    size_t m_peak;

private:
    struct AllocationInfo {
        size_t blockSize;
        uint32_t padding;
        bool isAllocated;
    };

    using Node = DoublyLinkedList<AllocationInfo>::Node;

    void *m_start_ptr = nullptr;
    PlacementPolicy m_pPolicy;
    DoublyLinkedList<AllocationInfo> m_blockList;
    size_t m_alignment;

    // std::set<FreeNodeInfo, CMPFreeNodeInfo, PoolAllocator<FreeNodeInfo> > m_freeNodeSet;
    std::set<FreeNodeInfo, CMPFreeNodeInfo> m_freeNodeSet;
    #ifdef DEBUG_ALLOCATOR
    std::map<size_t, size_t> m_askedMemMap;
    #endif


public:
    BlockAllocator(const size_t totalSize = MAX_MEM_SIZE, const PlacementPolicy pPolicy = FIND_BEST, const size_t alignment = DEFAULT_ALIGNMENT);

    ~BlockAllocator();

    size_t GetMemUsed() const
    {
        return m_used;
    }

    #ifdef DEBUG_ALLOCATOR
    size_t GetMemAsked() const
    {
        return m_asked;
    }
    #endif

    void *Allocate(const size_t size, size_t alignment = 0);

    void Free(void *ptr);

    void Init();
    
    void Reset();

    void Info();

private:
    BlockAllocator(BlockAllocator &blockAllocator);

    void CoalescenceWithNextBlock(Node *curNode);

    Node *Find(const size_t size, const size_t alignment, uint32_t &padding);

    // Node *FindBest(const size_t size, const size_t alignment, size_t &padding);

    Node *FindBestInSet(const size_t size, const size_t alignment, uint32_t &padding);
    
    //void FindFirst(const size_t size, const size_t aligment, size_t &padding, Node *&previousNode, Node *&foundNode);

    size_t GetAlignedAddr(size_t baseAddr);

    uint32_t CalculatePadding(const size_t baseAddr, const size_t alignment);
        
    uint32_t CalculatePaddingWithHeader(const size_t baseAddr, const size_t alignment, const size_t headerSize);
};

#endif // BLOCKALLOCATOR_H
