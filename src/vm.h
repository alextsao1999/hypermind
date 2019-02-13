//
// Created by 曹顺 on 2019/2/13.
//

#ifndef HYPERMIND_VM_H
#define HYPERMIND_VM_H


#include <cstdint>
namespace hypermind {
    class VM {
    public:

        void *memManger(void *ptr, uint32_t oldSize, uint32_t newSize);

    protected:
        uint32_t allocatedBytes{0};



    };
}


#endif //HYPERMIND_VM_H
