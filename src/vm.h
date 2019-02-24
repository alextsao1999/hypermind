//
// Created by 曹顺 on 2019/2/13.
//

#ifndef HYPERMIND_VM_H
#define HYPERMIND_VM_H


#include "hypermind.h"
#include "obj/object.h"

namespace hypermind {
    struct HMObject;

    class VM {
    public:
        void *MemManger(void *ptr, size_t oldSize, size_t newSize);
        HMUINT32 GetAllocatedBytes();

        template <typename T>
        T *Allocate(HMUINT32 extraSize = 0){
            extraSize += sizeof(T);
            mAllocatedBytes += extraSize;
            return MemManger(nullptr, 0, extraSize);
        };

    protected:
        // 已经分配的字节数
        HMUINT32 mAllocatedBytes{0};
       HMObject *mAllObjects;


    };
}


#endif //HYPERMIND_VM_H
