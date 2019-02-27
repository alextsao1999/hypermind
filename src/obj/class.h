//
// Created by 曹顺 on 2019/2/23.
//

#ifndef HYPERMIND_CLASS_H
#define HYPERMIND_CLASS_H

#include "buffer.h"
#include "function.h"
#include "value.h"
namespace hypermind {
    // 原生函数指针
    typedef bool (*HMPrimitive)(VM *vm, Value *args);

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
            HMPrimitive *pfn; // 指向原生方法

        };
        HMHash hash() override;
        HMMethod(VM *vm, MethodType methodType, HMClosure *fn);
    };

    // 类对象
    struct HMClass : public HMObject {
        HMClass *superClass;  // 父类
        HMUINT32 fieldNumer;
        HMHash hash() override;
        Buffer<HMMethod> methods;
    };

    struct HMInstance : public HMObject {
        Value *fields{nullptr};
    };

}

#endif //HYPERMIND_CLASS_H
