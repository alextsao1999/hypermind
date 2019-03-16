//
// Created by 曹顺 on 2019/2/25.
//

#ifndef HYPERMIND_COMPILER_H
#define HYPERMIND_COMPILER_H

#include "vm.h"
#include "ast.h"
#include "obj/function.h"

#define VT_TO_VALUE(VT) {VT, 0}

namespace hypermind {
    class CompileUnit {
        friend ASTFunctionStmt;
    public:
        explicit CompileUnit(VM *mVm);

        /**
         * 声明变量
         * @param mIdentifier
         */
        void DeclareVariable(const Token &id) {

        };

        /**
         * 添加常量
         * @param value
         * @return  索引值
         */
        HMUINT32 AddConstant(const Value &value) {

        };

        /**
         * 压入字面量
         */
        void EmitPushValue(const Value &value) {

        };

        /**
         * 栈中压入变量
         */
        void EmitLoadVariable(const Token &id){

        };

        void EmitLoadConstant(HMUINT32 index) {

        }
    protected:
        // 作用域深度
        HMUINT32 mScopeDepth;

        // 所属虚拟机
        VM *mVm;

        // 当前正在编译的函数
        HMFunction *mFn;

        // 符号表
        std::map<String, HMUINT32> mSymbolTable;

        HMUINT32 mStackSlotNum;

        // 外层编译单元
        CompileUnit *mOuter;

    };

}

#endif //HYPERMIND_COMPILER_H
