//
// Created by 曹顺 on 2019/2/25.
//

#ifndef HYPERMIND_COMPILER_H
#define HYPERMIND_COMPILER_H

#include "hypermind.h"
#include "vm.h"
#include "ast.h"
#include "obj/function.h"

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
        uint32_t index;
    };  //upvalue结构

    // 变量 表示局部/模块/Upval变量
    struct Variable {
        // 作用域深度
        ScopeType scopeType;
        // 索引
        HMUINT32 index;
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
    protected:
        // 所属虚拟机
        VM *mVM;
        // 作用域深度
        HMInteger mScopeDepth;

        // 当前正在编译的函数
        HMFunction *mFn;

        Upvalue upvalues[MAX_UPVALUE_NUMBER];
        LocalVariable localVariable[MAX_LOCAL_VAR_NUMBER];
        HMUINT32 localVarNumber; // 局部变量个数

        // 最大栈数量
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
        HMUINT32 DeclareLocalVariable(const Token &id) {
            for (HMUINT32 i = 0; i < localVarNumber; ++i) {
                if (id.mLength == localVariable[i].length &&
                    hm_memcmp(localVariable[i].name, id.mStart, id.mLength) != 0) {
                    return i;
                }
            }
            if (localVarNumber >= MAX_LOCAL_VAR_NUMBER) {
                // TODO 错误 变量数目大于最大局部变量
            }
            localVariable[localVarNumber].name = id.mStart;
            localVariable[localVarNumber].length = id.mLength;
            localVariable[localVarNumber].scopeDepth = mScopeDepth;
            localVariable[localVarNumber].isUpvalue = false;
            return localVarNumber++;
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
        HMUINT32 AddConstant(const Value &value) {
            // TODO 现在直接将value存到constants中了相同的文本或者数值会造成资源重复 可以先取hash
            return mFn->constants.Append(value);
        };

        /**
         * 操作栈中压入Null
         */
        void EmitPushNull() {

        };

        /**
         * 操作栈中压入变量
         * @param var 变量信息(作用域和索引)
         */
        void EmitLoadVariable(const Variable &var) {

        };

        /**
         * 操作栈中压入常量
         * @param index
         */
        void EmitLoadConstant(HMUINT32 index) {

        }

    };

}

#endif //HYPERMIND_COMPILER_H
