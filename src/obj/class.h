//
// Created by 曹顺 on 2019/2/23.
//

#ifndef HYPERMIND_CLASS_H
#define HYPERMIND_CLASS_H


#include "object.h"
#include "function.h"
namespace hypermind {
    enum class MethodType {
        None,     // 空方法类型, 并不等同于undefined
        Primitive,   // 原生方法
        Script,  // 脚本中定义的方法
        FunctionCall  // 有关函数对象的调用方法, 用来实现函数重载
    };

    // Method对象
    struct HMMethod : public HMObject {
        MethodType type;
        union {
            HMClosure *fn{};  // 方法引用的函数闭包
        };
        HMUINT32 hash() override;
    };

    // 类对象
    struct HMClass : public HMObject {
        HMClass *superClass;  // 父类
        HMUINT32 fieldNumer;
        Buffer<HMMethod> methods;

        HMUINT32 hash() override;

    };

}

#endif //HYPERMIND_CLASS_H
