//
// Created by 曹顺 on 2019/4/2.
//

#ifndef HYPERMIND_GC_H
#define HYPERMIND_GC_H

#include "hypermind.h"

namespace hypermind {
    struct HMObject;
    class GCHeap {
    protected:
        // 已经分配的字节数
        HMUINT32 mAllocatedBytes{0};
        
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
        HMObject *mProtectedObj[8];
        HMUINT32 mProtectedObjNum{0};

        void *MemManger(void *ptr, size_t oldSize, size_t newSize);

        inline void Gray(HMObject *obj);
        
        /**
         * 标黑对象
         * @param obj
         */
        inline void Black(HMObject *obj);

        void PushProtectedObject(HMObject *object);

        void PopProtectedObject();

        void StartGC();


    };

}


#endif //HYPERMIND_GC_H
