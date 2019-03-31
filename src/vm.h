//
// Created by 曹顺 on 2019/2/13.
//

#ifndef HYPERMIND_VM_H
#define HYPERMIND_VM_H

#include <obj/object.h>
#include "hypermind.h"
#include "obj/value.h"
namespace hypermind {
    struct HMThread;
    class VM {
    public:
        /**
         * 链接的所有对象
         */
        HMObject *mAllObjects{nullptr};

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

        inline void Gray(HMObject *obj) {
            if (obj == nullptr || obj->isDark)
                return;
            obj->isDark = true;
            mGrayObjs.push_back(obj);
        }

        /**
         * 标黑对象
         * @param obj
         */
        inline void Black(HMObject *obj) {

        }

        inline void PushProtectedObject(HMObject *object) {
            mProtectedObj[mProtectedObjNum++] = object;
        }

        inline void PopProtectedObject() {
            mProtectedObjNum--;
        }

        void StartGC() {
            HMObject *&obj = mAllObjects;
            while (obj != nullptr) {
                if (obj->isDark) {
                    // 黑对象置白
                    obj->isDark = false;
                } else {
                    // 白对象清除
//                    obj->free(this);
//                    Deallocate(obj, sizeof());

                }
            }

        }

    protected:
        // 已经分配的字节数
        HMUINT32 mAllocatedBytes{0};

        // GC 配置
        struct {
            //堆生长因子
            float heapGrowthFactor{1.5};

            //初始堆大小,默认为10MB
            HMUINT32 initialHeapSize;

            //最小堆大小,默认为1MB
            HMUINT32 minHeapSize;

            //第一次触发gc的堆大小,默认为initialHeapSize
            HMUINT32 nextGC;
        } mConfig;

        // 所有灰对象
        Vector<HMObject *> mGrayObjs;
        // 受保护的对象
        HMObject *mProtectedObj[8];
        HMUINT32 mProtectedObjNum{0};

    };
}


#endif //HYPERMIND_VM_H
