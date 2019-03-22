//
// Created by 曹顺 on 2019/2/25.
//

#ifndef HYPERMIND_COMPILER_H
#define HYPERMIND_COMPILER_H

#include "hypermind.h"
#include "vm.h"
#include "ast.h"
#include "obj/function.h"
#include "opcode.h"

// 操作栈改变 影响栈的深度
#define STACK_CHANGE(num) {mStackSlotNum += num;}

#define VT_TO_VALUE(VT) {VT, 0}

namespace hypermind {
    enum class ScopeType {
        Invalid,
        Module,  // 模块变量
        Local, // 局部变量
        Upval
    };

    struct Upvalue {
        // 是否为外层的局部变量
        bool isOuterLocalVar;

        //外层函数中局部变量的索引或者外层函数中upvalue的索引
        HMInteger index;
    };  //upvalue结构

    // 变量 表示局部/模块/Upval变量
    struct Variable {
        // 作用域深度
        ScopeType scopeType;
        // 索引
        HMInteger index;
    };

    // 局部变量
    struct LocalVariable {
        const HMChar *name;
        HMUINT32 length;
        HMInteger scopeDepth;  //局部变量作用域
        bool isUpvalue;
    };


    class CompileUnit {
        friend ASTFunctionStmt;
        friend ASTVarStmt;
        friend ASTVariable;
    protected:
        // 所属虚拟机
        VM *mVM;
        // 作用域深度
        HMInteger mScopeDepth;

        // 当前正在编译的函数
        HMFunction *mFn;

        Upvalue mUpvalues[MAX_UPVALUE_NUMBER];
        HMUINT32 mUpvalueNumber; // Upval 个数

        LocalVariable mLocalVariables[MAX_LOCAL_VAR_NUMBER];
        HMUINT32 mLocalVarNumber; // 局部变量个数

        // 最大操作栈数量
        HMUINT32 mStackSlotNum;

        // 外层编译单元
        CompileUnit *mOuter;

    public:
        explicit CompileUnit(VM *mVm);

        /**
         * 当前作用域声明局部变量 存在返回索引 不存在添加后返回索引
         * @param id
         * @return 返回索引
         */
        HMInteger DeclareLocalVariable(const Token &id) {
            for (HMUINT32 i = 0; i < mLocalVarNumber; ++i) {
                if (id.mLength == mLocalVariables[i].length &&
                    hm_memcmp(mLocalVariables[i].name, id.mStart, id.mLength) != 0) {
                    return i;
                }
            }
            if (mLocalVarNumber >= MAX_LOCAL_VAR_NUMBER) {
                // TODO 错误 变量数目大于最大局部变量
            }
            mLocalVariables[mLocalVarNumber].name = id.mStart;
            mLocalVariables[mLocalVarNumber].length = id.mLength;
            mLocalVariables[mLocalVarNumber].scopeDepth = mScopeDepth;
            mLocalVariables[mLocalVarNumber].isUpvalue = false;
            return mLocalVarNumber++;
        };

        /**
         * 查找变量
         * @param id
         * @return
         */
        Variable FindVariable(const Token &id) {
            Variable var{};
            var.scopeType = ScopeType::Local;
            var.index = DeclareLocalVariable(id);
            return var;
        };

        HMInteger AddUpval(bool isEnclosingLocalVar, HMInteger index) {
            for (HMInteger i = 0; i < mUpvalueNumber; ++i) {
                if (mUpvalues[i].index == index)
                    return i;
            }
            return -1;
        };

        /**
         * 设置变量为Upval
         * @param index
         */
        void SetLocalVariableUpval(HMInteger index) {
            mLocalVariables[index].isUpvalue = true;
        };


        // 进入作用域
        void EnterScope() {
            mScopeDepth++;
        };

        // 离开作用域
        void LeaveScope() {
            mScopeDepth--;

        }

        /**
         * 添加常量
         * @param value
         * @return  索引值
         */
        HMInteger AddConstant(const Value &value) {
            // TODO 现在直接将value存到constants中了相同的文本或者数值会造成资源重复 可以先取hash
            return mFn->constants.Append(value);
        };

        /**
         * 操作栈中压入Null
         */
        void EmitPushNull() {
            STACK_CHANGE(1);
            mFn->WriteOpcode(Opcode::PushNull);
        };

        /**
         * 栈中压入True
         */
        void EmitPushTrue() {
            STACK_CHANGE(1);
            mFn->WriteOpcode(Opcode::PushNull);
        };

        /**
         * 栈中压入False
         */
        void EmitPushFalse() {
            STACK_CHANGE(1);
            mFn->WriteOpcode(Opcode::PushFalse);
        };

        /**
         * 弹出栈顶参数
         * @param argNum
         */
        void EmitCall(HMUINT32 argNum) {
            STACK_CHANGE(argNum);

        }

        /**
         * 操作栈中压入变量
         * @param var 变量信息(作用域和索引)
         */
        void EmitLoadVariable(const Variable &var) {
            STACK_CHANGE(1);

        };

        /**
         * 操作栈中压入常量
         * @param index
         */
        void EmitLoadConstant(HMInteger index) {
            STACK_CHANGE(1);
            mFn->WriteOpcode(Opcode::LoadConstant);
            mFn->WriteShortOperand(index);

        }

    };

    class Compiler {

    private:

    public:
        // 当前正在编译的模块
        HMModule *mCurModule;
        // 当前正在编译的函数
        CompileUnit *mCurCompileUnit;

    };

}

#endif //HYPERMIND_COMPILER_H
