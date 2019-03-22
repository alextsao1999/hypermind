//
// Created by 曹顺 on 2019/2/13.
//

#ifndef HYPERMIND_VM_H
#define HYPERMIND_VM_H

#include "hypermind.h"

namespace hypermind {
    class VM {
    public:
        /**
         * 链接的所有对象
         */
        HMObject *mAllObjects{nullptr};
        HMClass *mBooleanClass{nullptr};
        HMClass *mStringClass{nullptr};
        HMClass *mIntegerClass{nullptr};
        HMClass *mDoubleClass{nullptr};
        HMClass *mFunctionClass{nullptr};
        HMClass *mMethodClass{nullptr};
        HMClass *mMetaClass{nullptr};
        HMClass *mMapClass{nullptr};
        HMClass *mListClass{nullptr};
        /**
         * 虚拟机内存管理
         * 频繁的申请内存可以用内存池代替
         *  记录内存变化
         * @param ptr 指针
         * @param oldSize 原长度
         * @param newSize  新长度
         * @return
         */
        void *MemManger(void *ptr, size_t oldSize, size_t newSize);
        HMUINT32 GetAllocatedBytes();

        /**
         * 在虚拟机中申请内存
         * @tparam T  申请类型
         * @param extraSize 附加长度
         * @return
         */
        template <typename T>
        T *Allocate(HMUINT32 count = 0){
            return (T *) MemManger(nullptr, 0, sizeof(T) * count);
        };

        void *Allocate(HMUINT32 size) {
            return MemManger(nullptr, 0, size);
        }

        void *Allocate(HMUINT32 elementSize, HMUINT32 count) {
            return MemManger(nullptr, 0, elementSize * count);
        }

        void *Deallocate(void *ptr, HMUINT32 size){
            return MemManger(ptr, size, 0);
        };

        void LinkObject(HMObject *obj);
        void DumpAllObjects();

        template<typename T, typename ...Args>
        T *New(Args&&...args) {
            return new(MemManger(nullptr, 0, sizeof(T))) T(std::forward<Args>(args)...);
        }

        template<typename T, typename ...Args>
        T *NewObject(Args&&...args) {
            return new(MemManger(nullptr, 0, sizeof(T))) T(this, std::forward<Args>(args)...);
        }

    protected:
        // 已经分配的字节数
        HMUINT32 mAllocatedBytes{0};


    };
}


#endif //HYPERMIND_VM_H
