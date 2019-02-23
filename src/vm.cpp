//
// Created by 曹顺 on 2019/2/13.
//

#include <cstdlib>
#include "vm.h"

namespace hypermind {
    /**
     * 虚拟机内存管理
     * 频繁的申请内存可以用内存池代替
     *  记录内存变化
     * @param ptr 指针
     * @param oldSize 原长度
     * @param newSize  新长度
     * @return
     */
    void *VM::memManger(void *ptr, size_t oldSize, size_t newSize) {
        allocatedBytes += newSize - oldSize;
        if (newSize == 0){
            free(ptr);
            return nullptr; // 防止野指针
        }

        return realloc(ptr, newSize);

    }

    uint32_t VM::getAllocatedBytes() {
        return allocatedBytes;
    }
}