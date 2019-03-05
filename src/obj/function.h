//
// Created by 曹顺 on 2019/2/23.
//

#ifndef HYPERMIND_FUNCTION_H
#define HYPERMIND_FUNCTION_H

#include "hypermind.h"
#include "buffer.h"
#include "value.h"
#include "module.h"
#include "object.h"

namespace hypermind {
    struct HMObject;
    // 函数对象
    HM_OBJECT(Function) {
        Buffer<HMByte> instructions; // 指令流
        Buffer<Value> constants;  // 所有常量
        HMModule *module;   // 所属模块
        HMUINT32 maxStackNumber{};  // 最大栈空间
        HMUINT32 upvalueNumber{}; // upval 数量
        HM_OBJ_DECL();
#ifdef DEBUG
        FunctionDebug *debug;
#endif
    };

    // Upvalue对象
    HM_OBJECT(Upvalue) {
        Value *localVarPtr;
        Value closedUpvalue;

        // 链接下一个Upvalue
        HMUpvalue *nextUpvalue;
        HM_OBJ_DECL();
    };

    // 闭包对象
    HM_OBJECT(Closure) {
        // 闭包指向的函数
        HMFunction *pFun;
        HM_OBJ_DECL();
    };

    struct Frame {
        // 指向下一条指令
        HMByte *ip;
        // 指向的闭包对象
        HMClosure *closure;
        Value *stackStart;
    };

    // 调试结构
    struct FunctionDebug {
        HMChar *name;  // 函数名称
        Buffer<HMInteger>line;  // 所在行号
    };

}

#endif //HYPERMIND_FUNCTION_H
