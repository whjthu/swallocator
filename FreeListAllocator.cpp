#include "FreeListAllocator.h"
#include "Utils.h"
#include <cstdlib>
#include <cassert>
#include <limits>
#include <algorithm>

// #define _DEBUG
#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

FreeListAllocator::FreeListAllocator(const size_t totalSize, const PlacementPolicy pPolicy) 
    : Allocator(totalSize) {
    m_pPolicy = pPolicy;
}

void FreeListAllocator::Init() {
    if (m_start_ptr != nullptr) {
        free(m_start_ptr);
        m_start_ptr = nullptr;
    }
    m_start_ptr = malloc(m_totalSize);
    this->Reset();
}

FreeListAllocator::~FreeListAllocator() {
    if (m_start_ptr != nullptr) {
        free(m_start_ptr);
        m_start_ptr = nullptr;
    }
}

// The allocated memory is organized as below:
// [prev block] [| padding | header | aligned data | rest |] [next block]
// So we have: 
// allocation_size = padding + header + data_size + rest
// free_node_addr = header_addr - padding
// header_addr = data_addr - header_size
// next_block_addr = free_node_addr + allocation_size
void *FreeListAllocator::Allocate(const size_t size, const size_t alignment) {
    auto headerSize = sizeof(AllocationHeader);
    auto nodeSize = sizeof(Node);
    Node *foundNode = nullptr,
         *prevNode = nullptr;
    size_t padding = 0;
    Find(size, alignment, padding, prevNode, foundNode);
    assert(foundNode != nullptr && "No enough memory");

    auto allocationSize = size + padding + headerSize;
#ifdef _DEBUG
    std::cout << "A0 " << "\tS " << size << "\tBS " << foundNode->data.blockSize 
              << "\tAS " << allocationSize << std::endl;
#endif // _DEBUG
    assert(foundNode->data.blockSize >= allocationSize);
    auto rest = foundNode->data.blockSize - allocationSize;
    if (rest >= sizeof(Node)) {
        Node *newFreeNode = (Node *)((size_t)foundNode + allocationSize);
        newFreeNode->data.blockSize = rest;
        m_freeList.insert(foundNode, newFreeNode);
    } else {
        allocationSize += rest;
    }
    m_freeList.remove(prevNode, foundNode);

    size_t headerAddr = (size_t)foundNode + padding;
    size_t dataAddr = headerAddr + headerSize;
    auto header = (AllocationHeader *)headerAddr;
    header->blockSize = allocationSize;
    header->padding = padding;

    m_used += allocationSize;
    m_peak = std::max(m_peak, m_used);

#ifdef _DEBUG
    std::cout << "A" << "\tH@ " << (void *) headerAddr << "\tD@ " << (void *) dataAddr
              << "\tAS " << size
              << "\tBS " << ((FreeListAllocator::AllocationHeader *) headerAddr)->blockSize
              << "\tP " << padding << "\tM " << m_used << "\tR " << rest 
              << std::endl;
#endif // _DEBUG

    return (void*)dataAddr;
}

void FreeListAllocator::Find(const size_t size, const size_t alignment, size_t &padding,
                             Node *&previousNode, Node *&foundNode) {
    switch (m_pPolicy) {
        case FIND_FIRST: {
            FindFirst(size, alignment, padding, previousNode, foundNode);
            break;
        }
        case FIND_BEST: {
            FindBest(size, alignment, padding, previousNode, foundNode);
            break;
        }
    }
}

void FreeListAllocator::FindFirst(const size_t size, const size_t alignment, size_t &padding, 
                                  Node *&previousNode, Node *&foundNode) {
    Node *it = m_freeList.head,
         *itPrev = nullptr;
    auto headerSize = sizeof(FreeListAllocator::AllocationHeader);
    while (it != nullptr) {
        auto bs = it->data.blockSize;
        auto padding = Utils::CalculatePaddingWithHeader(
                (size_t)it, alignment, headerSize);
        auto neededSize = size + padding + headerSize;
        if (bs >= neededSize) {
            break;
        }
        itPrev = it;
        it = it->next;
    }
    previousNode = itPrev;
    foundNode = it;
}

void FreeListAllocator::FindBest(const size_t size, const size_t alignment, size_t &padding, 
                                 Node *&previousNode, Node *&foundNode) {
    Node *it = m_freeList.head,
         *itPrev = nullptr,
         *best = nullptr,
         *bestPrev = nullptr;
    size_t smallestDiff = std::numeric_limits<size_t>::max();
    auto headerSize = sizeof(FreeListAllocator::AllocationHeader);
    while (it != nullptr) {
        auto bs = it->data.blockSize;
        auto padding = Utils::CalculatePaddingWithHeader(
                (size_t)it, alignment, headerSize);
        auto neededSize = size + padding + headerSize;
        auto diff = bs - neededSize;
        if (bs >= neededSize && diff < smallestDiff) {
            best = it;
            bestPrev= itPrev;
            smallestDiff = diff;
        }
        itPrev = it;
        it = it->next;
    }
    previousNode = bestPrev;
    foundNode = best;
}

void FreeListAllocator::Free(void *ptr) {
    size_t dataAddr = (size_t)ptr;
    size_t headerAddr = dataAddr - sizeof(AllocationHeader);
    AllocationHeader *header = (AllocationHeader *)headerAddr;
    size_t allocationSize = header->blockSize,
           padding = header->padding;
    Node *freeNode = (Node *)(headerAddr - padding);
    freeNode->data.blockSize = allocationSize;
    freeNode->next = nullptr;

    Node *it = m_freeList.head,
         *itPrev = nullptr;
    while (it != nullptr) {
        if (ptr < it) {
            break;
        }
        itPrev = it;
        it = it->next;
    }
    m_freeList.insert(itPrev, freeNode);

    m_used -= freeNode->data.blockSize;

    Coalescence(itPrev, freeNode);

#ifdef _DEBUG
    std::cout << "F" << "\tptr@ " << ptr << "\tH@ " << (void *)freeNode 
              << "\tS " << freeNode->data.blockSize << "\tM " << m_used << std::endl;
#endif // _DEBUG
}

void FreeListAllocator::Coalescence(Node *previousNode, Node *freeNode) {
    if (freeNode->next != nullptr && 
        (size_t)freeNode + freeNode->data.blockSize == (size_t) freeNode->next) {
        freeNode->data.blockSize += freeNode->next->data.blockSize;
        m_freeList.remove(freeNode, freeNode->next);
#ifdef _DEBUG
        std::cout << "\tMerging(n) " << (void *)freeNode << " & " << (void*)freeNode->next
                  << "\tS " << freeNode->data.blockSize << std::endl;
#endif // _DEBUG
    }

    if (previousNode != nullptr && 
        (size_t)previousNode + previousNode->data.blockSize == (size_t)freeNode) {
        previousNode->data.blockSize += freeNode->data.blockSize;
        m_freeList.remove(previousNode, freeNode);
#ifdef _DEBUG
        std::cout << "\tMerging(p) " << (void *)previousNode << " & " << (void*)freeNode
                  << "\tS " << previousNode->data.blockSize << std::endl;
#endif // _DEBUG
    }
}

void FreeListAllocator::Reset() {
    m_used = 0;
    m_peak = 0;
    Node *firstNode = (Node *) m_start_ptr;
    firstNode->data.blockSize = m_totalSize;
    firstNode->next = nullptr;
    m_freeList.head = nullptr;
    m_freeList.insert(nullptr, firstNode);
}
