//
// Created by 曹顺 on 2019/2/19.
//

#ifndef HYPERMIND_OBJECT_H
#define HYPERMIND_OBJECT_H

#include "hypermind.h"
#include "buffer.h"

namespace hypermind {
    struct HMObject;
    struct HMClass;

    enum class ObjectType {
        Class,   //此项是class类型,以下都是object类型
        List,
        Map,
        Module,
        Range,
        String,
        Upval,
        Function,
        Closure,
        Instance,
        Thread
    };

    // Object 是一切对象的基类
    struct HMObject {
        ObjectType type;
        bool isDark{false};  // 是否可达
        HMClass *classObj;
        HMObject *next; // 链接下一个Object
        HMObject(VM *vm, ObjectType type, HMClass *classObj);

        // 获得对象hash值
        virtual HMHash hash() = 0;

        // 释放对象内存
        virtual bool release();

    };


}


#endif //HYPERMIND_OBJECT_H
