//
// Created by 曹顺 on 2019/2/19.
//

#ifndef HYPERMIND_OBJECT_H
#define HYPERMIND_OBJECT_H

#include "hypermind.h"
#include "vm.h"
namespace hypermind {
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
        Method,
        Closure,
        Instance,
        Thread
    };
    // Object 是一切对象的基类
    struct HMObject {
        ObjectType type{};
        bool isDark{false};  // 是否可达
        HMClass *classObj{};
        HMObject *next{}; // 链接下一个Object

        HMObject() = default;
        HMObject(ObjectType type, HMClass *classObj) : type(type), classObj(classObj) {};

        /**
         * 自动将对象链接到虚拟机中
         * @param vm
         * @param type
         * @param classObj
         */
        HMObject(VM *vm, ObjectType type, HMClass *classObj);

        /**
         * 获得对象hash值
         * @return hash值
         */
        virtual HMHash hash() = 0;

        virtual void dump(){};

        /**
         * 释放对象内存
         * @return bool
         */
        virtual HMBool release(VM *vm){
            return true;
        };
    };


}
#endif //HYPERMIND_OBJECT_H
