//
// Created by ��˳ on 2019/3/5.
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
        HMUINT32 usedFrameNum{};   //��ʹ�õ�frame����
        HMUINT32 frameCapacity{};  //frame����

        // ������
        HMThread *caller{};
        // �������
        Value errorObj{};
        HM_OBJ_DECL();

    };

}


#endif //PROJECT_THREAD_H
