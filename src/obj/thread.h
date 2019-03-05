//
// Created by 曹顺 on 2019/3/5.
//

#ifndef PROJECT_THREAD_H
#define PROJECT_THREAD_H

#include "object.h"
#include "value.h"
namespace hypermind {
    HM_OBJECT(Thread) {
        Value *stack{};
        Value *sp{};

        HMUINT32 stackCapacity{};
        HMUINT32 usedFrameNum{};   //已使用的frame数量
        HMUINT32 frameCapacity{};  //frame容量

        // 调用者
        HMThread *caller{};
        // 错误对象
        Value errorObj{};
        HM_OBJ_DECL();

    };

}


#endif //PROJECT_THREAD_H
