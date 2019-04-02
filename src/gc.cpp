//
// Created by 曹顺 on 2019/4/2.
//

#include "gc.h"
#include "obj/object.h"

namespace hypermind {
    void GCHeap::Gray(hypermind::HMObject *obj) {
        if (obj == nullptr || obj->isDark)
            return;
        obj->isDark = true;
        mGrayObjs.push_back(obj);
    }

    void *GCHeap::MemManger(void *ptr, size_t oldSize, size_t newSize) {
        mAllocatedBytes += newSize - oldSize;
        if (newSize == 0){
            free(ptr);
            return nullptr; // 防止野指针
        }
        return realloc(ptr, newSize);
    }
    


    void GCHeap::Black(HMObject *obj) {

    }

    void GCHeap::PushProtectedObject(HMObject *object) {
        mProtectedObj[mProtectedObjNum++] = object;
    }

    void GCHeap::PopProtectedObject() {
        mProtectedObjNum--;
    }

    void GCHeap::StartGC() {
        HMObject *&obj = mAllObjects;
        while (obj != nullptr) {
            if (obj->isDark) {
                // 黑对象置白
                obj->isDark = false;
            } else {
                // 白对象清除
//                    obj->free(this);
//                    Deallocate(obj, sizeof(HMObject));
            }
        }
    }

}
