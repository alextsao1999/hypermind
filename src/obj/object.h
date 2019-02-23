//
// Created by 曹顺 on 2019/2/19.
//

#ifndef HYPERMIND_OBJECT_H
#define HYPERMIND_OBJECT_H

#include "hypermind.h"
#include "buffer.h"
#include "value.h"

namespace hypermind {
    struct Value;
    struct HMObject;
    struct HMClass;
    struct HMMethod;

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
    enum class MethodType {
        None,     // 空方法类型, 并不等同于undefined
        Primitive,   // 原生方法
        Script,  // 脚本中定义的方法
        FunctionCall  // 有关函数对象的调用方法, 用来实现函数重载
    };

    // Object 是一切对象的基类
    struct HMObject {
        ObjectType type;
        bool isDark;  // 是否可达
        HMClass *objClass;
        HMObject *next; // 链接下一个Object
    };

    // 模块对象
    struct HMModule : public HMObject {

    };

    // 类对象
    struct HMClass : public HMObject {
        HMClass *superClass;  // 父类
        HMUINT32 fieldNumer;
        Buffer<HMMethod> methods;
    };

    // 闭包对象
    struct HMClosure : public HMObject {

    };

    // 函数对象
    struct HMFunction : public HMObject {
        Buffer<HMByte> instructions; // 指令流
        Buffer<Value> constants;  // 常量表
        HMModule *module;   // 所属模块
        HMUINT32 maxStackNumber;  // 最大栈空间
        HMUINT32 upvalueNumber; // upval 数量
#ifdef DEBUG
        FunctionDebug *debug;
#endif
    };

    // Upvalue对象
    struct HMUpvalue : public HMObject {

    };

    // Method对象
    struct HMMethod : public HMObject {
        MethodType type;
        union {
            HMClosure *fn;  // 方法引用的函数闭包
        };
    };

    // 调试结构
    struct FunctionDebug {
        HMChar *name;  // 函数名称
        Buffer<int>line;  // 所在行号
    };

}


#endif //HYPERMIND_OBJECT_H
