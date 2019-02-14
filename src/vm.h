//
// Created by 曹顺 on 2019/2/13.
//

#ifndef HYPERMIND_VM_H
#define HYPERMIND_VM_H


#include <cstdint>
namespace hypermind {
    class VM {
    public:
        void *memManger(void *ptr, size_t oldSize, size_t newSize);
        uint32_t getAllocatedBytes();
    protected:
        // 已经分配的字节数
        uint32_t allocatedBytes{0};

    };
}


#endif //HYPERMIND_VM_H
