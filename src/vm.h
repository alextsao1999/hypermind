//
// Created by 曹顺 on 2019/2/13.
//

#ifndef HYPERMIND_VM_H
#define HYPERMIND_VM_H

#include "hypermind.h"
#include "gc.h"
#include "symbol.h"

namespace hypermind {
    struct HMThread;
    class VM {
    public:
        HMClass *mBooleanClass{nullptr};
        HMClass *mStringClass{nullptr};
        HMClass *mIntegerClass{nullptr};
        HMClass *mFloatClass{nullptr};
        HMClass *mFunctionClass{nullptr};
        HMClass *mThreadClass{nullptr};
        HMClass *mMetaClass{nullptr};
        HMClass *mMapClass{nullptr};
        HMClass *mListClass{nullptr};
        HMThread *mCurrentThread{nullptr};

        SymbolTable mAllMethods;

        GCHeap mGCHeap;

        template<typename T, typename ...Args>
        T *New(Args&&...args) {
            return new(mGCHeap.MemManger(nullptr, 0, sizeof(T))) T(std::forward<Args>(args)...);
        }

        template<typename T, typename ...Args>
        T *NewObject(Args&&...args) {
            return new(mGCHeap.MemManger(nullptr, 0, sizeof(T))) T(this, std::forward<Args>(args)...);
        }

        void *Allocate(HMUINT32 size) {
            return mGCHeap.MemManger(nullptr, 0, size);
        }

        template<typename T>
        T *Allocate(HMUINT32 count) {
            return (T *) mGCHeap.MemManger(nullptr, 0, sizeof(T) * count);
        }

        void *Deallocate(void *ptr, HMUINT32 size) {
            return mGCHeap.MemManger(ptr, size, 0);
        }


    };
}


#endif //HYPERMIND_VM_H
