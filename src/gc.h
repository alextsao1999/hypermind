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
    struct HMThread;
    struct HMUpvalue;
    struct Value;
    template <typename T>
    class Buffer;
    class GCHeap {
    protected:
        // 已经分配的字节数
        HMUINT32 mAllocatedBytes{0};

        // 清理中字节数
        HMUINT32 mScaningBytes{0};
    public:
        VM *mVM;

        HMObject *mAllObjects{nullptr};

        //堆生长因子
        float mHeapGrowthFactor{1.5};

        //最小堆大小,默认为1MB
        HMUINT32 mMinHeapSize{1024 * 1024};

        // 下一次触发GC的阈值
        HMUINT32 mNextGC{1024 * 1024 * 10};

        explicit GCHeap(VM *vm);

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
        void Gray(HMObject *obj);
        void Gray(Value obj);

        /**
         * 标黑对象
         * @param obj
         */
        void Black(HMObject *obj);
        inline void BlackClass(HMClass *clazz);
        inline void BlackMap(HMMap *map);
        inline void BlackClosure(HMClosure *closure);
        inline void BlackFunction(HMFunction *function);
        inline void BlackModule(HMModule *module);
        inline void BlackInstance(HMInstance *instance);
        inline void BlackString(HMString *string);
        inline void BlackThread(HMThread *thread);
        inline void BlackUpvalue(HMUpvalue *upvalue);

        void PushProtectedObject(HMObject *object);

        void PopProtectedObject();

        void StartGC();

    };

}


#endif //HYPERMIND_GC_H
