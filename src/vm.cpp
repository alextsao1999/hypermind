//
// Created by 曹顺 on 2019/2/13.
//


#include "vm.h"

namespace hypermind {
    void *VM::MemManger(void *ptr, size_t oldSize, size_t newSize) {
        mAllocatedBytes += newSize - oldSize;
        if (newSize == 0){
            free(ptr);
            return nullptr; // 防止野指针
        }
        return realloc(ptr, newSize);
    }

    HMUINT32 VM::GetAllocatedBytes() {
        return mAllocatedBytes;
    }

    void VM::LinkObject(HMObject *obj) {
        obj->next = mAllObjects;
        mAllObjects->next = obj;
    }
}
