#include "BlockAllocator.h"
#include <cstdlib>
#include <cassert>
#include <limits>
#include <algorithm>
#include <mutex>

// #define _DEBUG
#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

std::mutex allocator_lock_;

BlockAllocator::BlockAllocator(const size_t totalSize, const PlacementPolicy pPolicy, const size_t alignment) {
    m_totalSize = totalSize;
    m_used = m_peak = 0;
    #ifdef DEBUG_ALLOCATOR
    m_asked = 0;
    #endif
    m_pPolicy = pPolicy;
    m_alignment = alignment;
    m_start_ptr = nullptr;
    this->Init();
}

void BlockAllocator::Init() {
    if (m_start_ptr != nullptr) {
        free(m_start_ptr);
        m_start_ptr = nullptr;
    }
    m_start_ptr = malloc(m_totalSize);
    this->Reset();
}

size_t BlockAllocator::GetAlignedAddr(size_t baseAddr) {
    return ((baseAddr - 1) / m_alignment + 1) * m_alignment;
}

void BlockAllocator::Reset() {
    // size_t firstDataAddr = GetAlignedAddr((size_t) m_start_ptr + sizeof(Node));
    // Node *firstNode = (Node *) (firstDataAddr - sizeof(Node));
    Node *firstNode = (Node *)m_start_ptr;

    // m_used = m_peak = ((size_t) firstNode) - ((size_t) m_start_ptr);
    m_used = 0;
    m_peak = 0;
    #ifdef DEBUG_ALLOCATOR
    m_asked = 0;
    #endif
    
    firstNode->data.blockSize = m_totalSize - m_used;
    firstNode->data.padding = 0;
    firstNode->data.isAllocated = false;
    firstNode->next = nullptr;
    firstNode->prev = nullptr;
    m_blockList.head = nullptr;
    m_blockList.tail = nullptr;
    m_blockList.insert(nullptr, firstNode);
    m_freeNodeSet.clear();
    m_freeNodeSet.emplace((size_t) firstNode, firstNode->data.blockSize);
    #ifdef DEBUG_ALLOCATOR
    m_askedMemMap.clear();
    #endif
}

BlockAllocator::~BlockAllocator() {
    if (m_start_ptr != nullptr) {
        free(m_start_ptr);
        m_start_ptr = nullptr;
    }
}

// The allocated memory is organized as below:
// [prev block] [| padding | header | aligned data | rest |] [next block]
// So we have: 
// allocation_size = padding + header_size + data_size + rest
// block_start = header_addr - padding
// header_addr = data_ptr - header_size
// next_block_addr = block_start + allocation_size
void *BlockAllocator::Allocate(const size_t size, size_t alignment) {
    std::lock_guard<std::mutex> lock_(allocator_lock_);
    if (size <= SMALL_ALLOCATION_SIZE) {
#ifdef _DEBUG
        std::cout << "Allocate: " << size << "B memory using malloc" << std::endl;
#endif // _DEBUG
        auto p = malloc(size);
        if (p == nullptr) {
            std::cout << "Malloc failed" << std::endl;
        }
        return p;
    }
#ifdef _DEBUG
    std::cout << "[Info] Allocated size: " << size << std::endl;
#endif // _DEBUG
    auto headerSize = sizeof(Node);
    alignment = alignment > 0 ? alignment : m_alignment;
    uint32_t padding = 0;
    Node *foundNode = Find(size, alignment, padding);
    if (foundNode == nullptr)
    {
        std::cout << "No enough memory!" << std::endl;
        std::cout << "Memory allocated: " << m_used  << "B" << std::endl;
        #ifdef DEBUG_ALLOCATOR
        std::cout << "Memory asked:     " << m_asked << "B" << std::endl;
        #endif
        return nullptr;
    }

    auto allocationSize = size + padding + headerSize;
#ifdef _DEBUG
    std::cout << "A0 " << "\tS " << size << "\tBS " << foundNode->data.blockSize 
              << "\tAS " << allocationSize << std::endl;
#endif // _DEBUG
    assert(foundNode->data.blockSize >= allocationSize);
    assert(foundNode->data.isAllocated == false);
    auto rest = foundNode->data.blockSize - allocationSize;
    if (rest > sizeof(Node)) {
        Node *newFreeNode = (Node *)((size_t)foundNode + allocationSize);
        newFreeNode->data.blockSize = rest;
        newFreeNode->data.isAllocated = false;
        m_blockList.insert(foundNode, newFreeNode);
        m_freeNodeSet.emplace((size_t) newFreeNode, newFreeNode->data.blockSize);
    }
    else {
        allocationSize += rest;
    }

    size_t blockAddr = (size_t)foundNode;
    size_t headerAddr = blockAddr + padding;
    size_t dataAddr = headerAddr + headerSize;
    auto prev = foundNode->prev;
    auto next = foundNode->next;
    auto headerNode = (Node*)headerAddr;
    headerNode->data.blockSize = allocationSize;
    headerNode->data.padding = padding;
    headerNode->data.isAllocated = true;
    headerNode->prev = prev;
    headerNode->next = next;
    if (headerNode->prev != nullptr)
        headerNode->prev->next = headerNode;
    if (headerNode->next != nullptr)
        headerNode->next->prev = headerNode;

    m_used += allocationSize;
    m_peak = std::max(m_peak, m_used);
    #ifdef DEBUG_ALLOCATOR
    m_asked += size;
    assert(m_askedMemMap.find(dataAddr) == m_askedMemMap.end());
    m_askedMemMap[dataAddr] = size;
    #endif


#ifdef _DEBUG
    std::cout << "A" << "\tH@ " << (void *) headerAddr << "\tD@ " << (void *) dataAddr
              << "\tB@ " << blockAddr
              << "\tAS " << size
              << "\tBS " << ((Node *) headerAddr)->data.blockSize
              << "\tP " << padding << "\tM " << m_used << "\tR " << rest 
              << std::endl;
#endif // _DEBUG

    return (void *)dataAddr;
}

BlockAllocator::Node *BlockAllocator::Find(const size_t size, const size_t alignment, uint32_t &padding) {
    /*switch (m_pPolicy) {
        //case FIND_FIRST: {
        //    FindFirst(size, alignment, padding, previousNode, foundNode);
        //    break;
        //}
        case FIND_BEST: {
            //return FindBest(size);
            return FindBestInSet(size);
            break;
        }
        default:
            assert(false);
    }*/
    return FindBestInSet(size, alignment, padding);
}

// BlockAllocator::Node *BlockAllocator::FindBest(const size_t size, const size_t alignment, size_t &padding) {
//     assert(false);
//     Node *it = m_blockList.head,
//          *best = nullptr;
//     size_t smallestDiff = std::numeric_limits<size_t>::max();
//     auto headerSize = sizeof(Node);
//     auto neededSize = size + headerSize;
//     while (it != nullptr) {
//         if (it->data.isAllocated == false)
//         {
//             auto bs = it->data.blockSize;
//             auto diff = bs - neededSize;
//             if (bs >= neededSize && diff < smallestDiff) {
//                 best = it;
//                 smallestDiff = diff;
//             }
//         }
//         it = it->next;
//     }
//     return best;
// }

BlockAllocator::Node *BlockAllocator::FindBestInSet(const size_t size, const size_t alignment, uint32_t &padding) {
#ifdef _DEBUG
    std::cout << "Finding best" << std::endl;
#endif // _DEBUG
    auto it = m_freeNodeSet.lower_bound(FreeNodeInfo(0, size + sizeof(Node) + alignment));
    auto headerSize = sizeof(Node);
    if (it != m_freeNodeSet.end()) {
        Node* retNode = (Node*) it->addr;
        padding = CalculatePaddingWithHeader((size_t)retNode, alignment, headerSize);
        auto neededSize = size + padding + headerSize;
        if (retNode->data.blockSize >= neededSize) {
            m_freeNodeSet.erase(it);
            // std::cout << "Found best" << std::endl;
            return retNode;
        } 
    }
    return nullptr;
}

/*

void BlockAllocator::FindFirst(const size_t size, const size_t alignment, size_t &padding, 
                                  Node *&previousNode, Node *&foundNode) {
    Node *it = m_freeList.head,
         *itPrev = nullptr;
    auto headerSize = sizeof(BlockAllocator::AllocationHeader);
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
*/

void BlockAllocator::Free(void *ptr) {
    std::lock_guard<std::mutex> lock_(allocator_lock_);
    if ((size_t)ptr < (size_t)m_start_ptr || (size_t)ptr >= ((size_t)m_start_ptr + (size_t)m_totalSize)) {
#ifdef _DEBUG
        std::cout << "Freeing " << ptr << " memory using free" << std::endl;
#endif // _DEBUG
        free(ptr);
        return;
    }
#ifdef _DEBUG
    std::cout << "Freeing " << "\tptr@ " << ptr << std::endl;
#endif // _DEBUG
    size_t dataAddr = (size_t)ptr;
    size_t headerAddr = dataAddr - sizeof(Node);
    Node *header = (Node *)headerAddr;
    auto padding = header->data.padding;
    auto blockSize = header->data.blockSize;
    auto prev = header->prev;
    auto next = header->next;
    Node *freeNode = (Node *) (headerAddr - padding);
    freeNode->data.blockSize = blockSize;
    freeNode->data.padding = 0;
    freeNode->data.isAllocated = false;
    freeNode->prev = prev;
    if (freeNode->prev != nullptr)
        freeNode->prev->next = freeNode;
    freeNode->next = next;
    if (freeNode->next != nullptr)
        freeNode->next->prev = freeNode;
    m_freeNodeSet.emplace((size_t) freeNode, blockSize);

    m_used -= freeNode->data.blockSize;
    #ifdef DEBUG_ALLOCATOR
    auto it = m_askedMemMap.find((size_t) ptr);
    assert(it != m_askedMemMap.end());
    m_asked -= it->second;
    std::cout << "Free " << it->second << "B    " << "Used " << m_asked << "B" << std::endl;
    m_askedMemMap.erase(it);
    #endif
    if (freeNode->next != nullptr && freeNode->next->data.isAllocated == false)
        CoalescenceWithNextBlock(freeNode);
    if (freeNode->prev != nullptr && freeNode->prev->data.isAllocated == false)
        CoalescenceWithNextBlock(freeNode->prev);

#ifdef _DEBUG
    std::cout << "F" << "\tptr@ " << ptr << "\tH@ " << (void *)freeNode 
              << "\tS " << freeNode->data.blockSize << "\tM " << m_used 
              << "\tB@ " << freeNode
              << std::endl;
#endif // _DEBUG
}

void BlockAllocator::CoalescenceWithNextBlock(Node *curNode) {
#ifdef _DEBUG
    std::cout << "Coalescence" << std::endl;
#endif // _DEBUG
    assert(curNode->next != nullptr);
    assert(curNode->next->data.isAllocated == false && curNode->data.isAllocated == false);
    assert((size_t)curNode + curNode->data.blockSize == (size_t) curNode->next);

    auto it = m_freeNodeSet.find(FreeNodeInfo((size_t) curNode, curNode->data.blockSize));
    assert(it != m_freeNodeSet.end());
    m_freeNodeSet.erase(it);

    it = m_freeNodeSet.find(FreeNodeInfo((size_t) curNode->next, curNode->next->data.blockSize));
    assert(it != m_freeNodeSet.end());
    m_freeNodeSet.erase(it);

    curNode->data.blockSize += curNode->next->data.blockSize;
    m_freeNodeSet.emplace((size_t) curNode, curNode->data.blockSize);
    m_blockList.remove(curNode->next);
#ifdef _DEBUG
    std::cout << "\tMerging(n) " << (void *)curNode << " & " << (void*)curNode->next
              << "\tS " << curNode->data.blockSize << std::endl;
#endif // _DEBUG
}

uint32_t BlockAllocator::CalculatePadding(const size_t baseAddr, const size_t alignment) {
    size_t multiplier = (baseAddr / alignment) + 1;
    size_t alignedAddr = multiplier * alignment;
    size_t padding = alignedAddr - baseAddr;
    return (uint32_t)padding;
}

uint32_t BlockAllocator::CalculatePaddingWithHeader(const size_t baseAddr, const size_t alignment, const size_t headerSize) {
    size_t padding = CalculatePadding(baseAddr, alignment);
    size_t neededSpace = headerSize;
    if (padding < neededSpace) {
        neededSpace -= padding;
        if (neededSpace % alignment > 0) {
            padding += alignment * (1 + (neededSpace / alignment));
        } else {
            padding += alignment * (neededSpace / alignment);
        }
    }
    return (uint32_t)(padding - headerSize);
}

void BlockAllocator::Info() {
    std::cout << "Used memory: " << m_used << ", peak memory: " << m_peak
              << ", total size: " << m_totalSize << std::endl;
}
