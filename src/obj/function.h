//
// Created by 曹顺 on 2019/2/23.
//

#ifndef HYPERMIND_FUNCTION_H
#define HYPERMIND_FUNCTION_H

#include "buffer.h"
#include "module.h"
#include "object.h"
#include "opcode.h"
namespace hypermind {
    struct Upvalue;
    // 调试结构
    struct FunctionDebug {
        String name;  // 函数名称
        Vector<HMUINT32> line;  // 所在行号
        explicit FunctionDebug(const String &name);
    };

    // 函数对象
    HM_OBJECT(Function) {
        Buffer<HMByte> instructions; // 指令流
        Buffer<Value> constants;  // 所有常量
        HMModule *module;   // 所属模块
        HMUINT32 maxStackSlotNum{0};  // 最大栈空间
        Upvalue *upvalues{nullptr};
        HMUINT32 upvalueNum{0}; // upvalue 数量
#ifdef HMDebug
        FunctionDebug *debug{nullptr};
#endif
        HM_OBJ_CONSTRUCTOR(Function, HMModule *module), constants(Buffer<Value>(vm)), instructions(Buffer<HMByte>(vm)),
                                                       module(module) {

        };

        HM_OBJ_DECL();

        void DumpInstruction(int start);

        void DumpAllInstructions(Ostream &os);

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
        HMFunction *pFn;
        HMUpvalue *upvalues{nullptr};
        HM_OBJ_CONSTRUCTOR_CLASS(Closure, vm->mFunctionClass, HMFunction *fun), pFn(fun), upvalues(
                vm->Allocate<HMUpvalue>(fun->upvalueNum)) {};

        HM_OBJ_DECL();
    };

    // 函数调用栈帧CallInfo
    struct Frame {
        // 指向下一条指令
        HMByte *ip;
        // 指向的闭包对象
        HMClosure *closure;
        Value *stackStart;
    };

}

#endif //HYPERMIND_FUNCTION_H
