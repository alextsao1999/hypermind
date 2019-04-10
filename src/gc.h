//
// Created by 曹顺 on 2019/4/2.
//

#ifndef HYPERMIND_GC_H
#define HYPERMIND_GC_H

#include "hypermind.h"
#define MAX_PROTECTED_OBJ 8
namespace hypermind {
    class VM;
    struct HMObject;
    struct HMClass;
    struct HMMap;
    struct HMFunction;
    struct HMClosure;
    struct HMModule;
    struct HMInstance;
    struct HMString;
    struct Value;
    template <typename T>
    class Buffer;
    class GCHeap {
    protected:
        // 已经分配的字节数
        HMUINT32 mAllocatedBytes{0};

        // 清理中字节数
        HMUINT32 mSweepingBytes{0};
    public:

        HMObject *mAllObjects{nullptr};

        //堆生长因子
        float mHeapGrowthFactor{1.5};

        //初始堆大小,默认为10MB
        HMUINT32 mInitialHeapSize{1024 * 1024 * 10};

        //最小堆大小,默认为1MB
        HMUINT32 mMinHeapSize;

        //第一次触发gc的堆大小,默认为initialHeapSize
        HMUINT32 mNextGC;

        // 所有灰对象
        Vector<HMObject *> mGrayObjs;
        // 受保护的对象
        HMObject *mProtectedObj[MAX_PROTECTED_OBJ];
        HMUINT32 mProtectedObjNum{0};

        void *MemManger(void *ptr, size_t oldSize, size_t newSize);

        /**
         * 将对象置灰
         * @param obj
         */
        inline void Gray(HMObject *obj);
        inline void Gray(Value obj);
        inline void Gray(Buffer<Value> values);

        inline void BlackGrayObjects(){
            while (!mGrayObjs.empty()) {
                Black(mGrayObjs.back());
                mGrayObjs.pop_back();
            }
        }

        /**
         * 标黑对象
         * @param obj
         */
        inline void Black(HMObject *obj);
        inline void BlackClass(HMClass *clazz);
        inline void BlackMap(HMMap *map);
        inline void BlackClosure(HMClosure *closure);
        inline void BlackFunction(HMFunction *function);
        inline void BlackModule(HMModule *module);
        inline void BlackInstance(HMInstance *instance);
        inline void BlackString(HMString *string);

        void PushProtectedObject(HMObject *object);

        void PopProtectedObject();

        void StartGC(VM *vm);


    };

}


#endif //HYPERMIND_GC_H
