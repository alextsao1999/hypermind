//
// Created by 曹顺 on 2019/2/23.
//

#ifndef HYPERMIND_FUNCTION_H
#define HYPERMIND_FUNCTION_H

#include "hypermind.h"
#include "object.h"
#include "buffer.h"
#include "module.h"
#include "value.h"
namespace hypermind {
    // 闭包对象
    struct HMClosure : public HMObject {
        HMHash hash() override;

    };

    // 函数对象
    struct HMFunction : public HMObject {
        Buffer<HMByte> instructions; // 指令流
        Buffer<Value> constants;  // 所有常量
        HMModule *module;   // 所属模块
        HMUINT32 maxStackNumber;  // 最大栈空间
        HMUINT32 upvalueNumber; // upval 数量
#ifdef DEBUG
        FunctionDebug *debug;
#endif

        HMHash hash() override;
    };

    // Upvalue对象
    struct HMUpvalue : public HMObject {
        HMHash hash() override;
    };


    // 调试结构
    struct FunctionDebug {
        HMChar *name;  // 函数名称
        Buffer<HMInteger>line;  // 所在行号
    };

}

#endif //HYPERMIND_FUNCTION_H
