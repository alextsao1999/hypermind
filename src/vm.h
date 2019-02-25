//
// Created by 曹顺 on 2019/2/13.
//

#ifndef HYPERMIND_VM_H
#define HYPERMIND_VM_H


#include "hypermind.h"
#include "obj/object.h"
#include "obj/class.h"
namespace hypermind {
    struct HMObject;
    struct HMClass;


    class VM {
    public:
        HMClass *mStringClass{nullptr};

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
        void LinkObject(HMObject *obj);

        /**
         * 在虚拟机中申请内存
         * @tparam T  申请类型
         * @param extraSize 附加长度
         * @return
         */
        template <typename T>
        T *Allocate(HMUINT32 extraSize = 0){
            extraSize += sizeof(T);
            return (T *) MemManger(nullptr, 0, extraSize);
        };

        void *Allocate(HMUINT32 size) {
            return MemManger(nullptr, 0, size);
        }
        void *Allocate(HMUINT32 elementSize, HMUINT32 count) {
            return MemManger(nullptr, 0, elementSize * count);
        }

    protected:
        // 已经分配的字节数
        HMUINT32 mAllocatedBytes{0};
        HMObject *mAllObjects{nullptr};

    };
}


#endif //HYPERMIND_VM_H
