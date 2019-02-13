//
// Created by 曹顺 on 2019/2/13.
//

#include <cstdlib>
#include "vm.h"

namespace hypermind {

    void *VM::memManger(void *ptr, uint32_t oldSize, uint32_t newSize) {
        allocatedBytes += newSize - oldSize;
        if (newSize == 0){
            free(ptr);
            return nullptr;
        }

        return realloc(ptr, newSize);

    }
}