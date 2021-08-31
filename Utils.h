#ifndef UTILS_H
#define UTILS_H

class Utils {
public:
    static const size_t CalculatePadding(const size_t baseAddr, const size_t alignment) {
        const size_t multiplier = (baseAddr / alignment) + 1;
        const size_t alignedAddr = multiplier * alignment;
        const size_t padding = alignedAddr - baseAddr;
        return padding;
    }

    static const size_t CalculatePaddingWithHeader(const size_t baseAddr, const size_t alignment, const size_t headerSize) {
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
        return padding - headerSize;
    }
};

#endif // UTILS_H
