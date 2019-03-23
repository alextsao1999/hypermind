//
// Created by 曹顺 on 2019/2/25.
//

#ifndef HYPERMIND_COMPILER_H
#define HYPERMIND_COMPILER_H

#include "hypermind.h"
#include "ast.h"
#include "obj/function.h"
#include "opcode.h"

// 操作栈改变 影响栈的深度
#define STACK_CHANGE(num) {mStackSlotNum += num;}

#define VT_TO_VALUE(VT) {VT, 0}

namespace hypermind {
    class VM;

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
    };  // upvalue结构

    // 变量 表示局部/模块/Upval变量
    struct Variable {
        // 作用域深度
        ScopeType scopeType;
        // 索引
        HMInteger index;
        explicit Variable(ScopeType type, HMInteger index) : scopeType(type), index(index) {};
        Variable() = default;
    };

    // 局部变量
    struct LocalVariable {
        const HMChar *name;
        HMUINT32 length;
        HMInteger scopeDepth;  //局部变量作用域
        bool isUpvalue;
    };

    // 方法签名
    struct MethodSignature {
        enum class SignatureType {
            Method,
            Getter,
            Setter,
            Subscript,
            SubscriptSetter
        };
        MethodSignature(SignatureType type, const HMChar *name, HMUINT32 length, HMInteger argNum) : type(type),
                                                                                                     name(name), length(length), argNum(argNum) {

        };
        MethodSignature() = default;

        SignatureType type;
        const HMChar *name;
        HMUINT32 length;
        HMInteger argNum;

    };

    class CompileUnit {
        friend Compiler;
    protected:
        // 所属虚拟机
        VM *mVM{nullptr};
        // 作用域深度
        HMInteger mScopeDepth{-1};

        // 当前正在编译的函数
        HMFunction *mFn;

        Upvalue mUpvalues[MAX_UPVALUE_NUMBER];
        HMUINT32 mUpvalueNumber{0}; // Upval 个数

        LocalVariable mLocalVariables[MAX_LOCAL_VAR_NUMBER];
        HMUINT32 mLocalVarNumber{0}; // 局部变量个数

        // 最大操作栈数量
        HMUINT32 mStackSlotNum{0};

    public:
        explicit CompileUnit(VM *mVm);

        // 外层编译单元
        CompileUnit *mOuter{nullptr};

        /**
         * 当前作用域声明局部变量 存在返回索引 不存在添加后返回索引
         * @param id
         * @return 返回索引
         */
        HMInteger DeclareLocalVariable(const Token &id) {
            HMInteger idx = FindLocal(id);
            // 此时变量已经存在了 返回存在的索引
            if (idx != -1)
                return idx;
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
         * 根据作用域声明变量/参数
         * @return
         */
        HMInteger DeclareVariable(const Token &id) {
            if (mScopeDepth == -1) {
                // TODO 模块变量
            }
            return DeclareLocalVariable(id);
        }

        /**
         * 定义变量   变量设置初始值
         * @param index
         */
        void DefineVariable(HMInteger index) {
            if (mScopeDepth == -1) {
                // 作用域为模块作用域
            }
            // 不是模块作用域 不用管
        }

        /**
         * 查找局部变量
         * @param id
         * @return
         */
        HMInteger FindLocal(const Token &id) {
            for (HMUINT32 i = 0; i < mLocalVarNumber; ++i) {
                if (id.mLength == mLocalVariables[i].length &&
                    hm_memcmp(mLocalVariables[i].name, id.mStart, id.mLength) == 0) {
                    return i;
                }
            }
            return -1;
        }

        // 查找局部变量 或者 Upvalue
        Variable FindLocalOrUpvalue(const Token &id) {
            // TODO 现在只是从局部变量中查找
            Variable var{};
            var.scopeType = ScopeType::Local;
            var.index = FindLocal(id);
            return var;
        }

        /**
         * 查找变量
         * @param id
         * @return
         */
        Variable FindVariable(const Token &id) {
            // TODO 现在只是从局部变量中查找 之后增加模块变量和Upvalue
            Variable var{};
            var.scopeType = ScopeType::Local;
            var.index = FindLocal(id);
            return var;
        };

        HMInteger AddUpvalue(bool isEnclosingLocalVar, HMInteger index) {
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
            DiscardLocalVariable();
            mScopeDepth--;
        }

        /**
         * 丢弃作用域内的局部变量
         */
        void DiscardLocalVariable() {

        }

        /**
         * 添加常量
         * @param value
         * @return  索引值
         */
        HMInteger AddConstant(const Value &value) {
            // TODO 现在直接将value存到constants中了
            //  相同的文本或者数值会造成资源重复 可以先取hash
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
            mFn->WriteOpcode(Opcode::PushTrue);
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
        void EmitCall(HMUINT32 methodIndex, HMUINT32 argNum) {
            STACK_CHANGE(argNum);
            if (argNum <= 7)
                mFn->WriteOpcode(static_cast<HMByte>((HMByte) Opcode::Call + argNum));
            else {
                mFn->WriteOpcode(Opcode::Call);
                mFn->WriteShortOperand(methodIndex); // 方法索引
                mFn->WriteShortOperand(argNum); // 实参数量
            }
        }

        /**
         * 操作栈中压入变量
         * @param var 变量信息(作用域和索引)
         */
        void EmitLoadVariable(const Variable &var) {
            STACK_CHANGE(1);
            switch (var.scopeType) {
                case ScopeType::Module:
                    mFn->WriteOpcode(Opcode::LoadModuleVariable);
                    break;
                case ScopeType::Local:
                    mFn->WriteOpcode(Opcode::LoadLocalVariable);
                    break;
                case ScopeType::Upval:
                    mFn->WriteOpcode(Opcode::LoadUpvalue);
                    break;
                case ScopeType::Invalid:
                    // FIXME
                    break;
            }
            mFn->WriteShortOperand(var.index);
        };

        /**
         * 储存变量
         * @param var
         */
        void EmitStoreVariable(const Variable &var) {
            STACK_CHANGE(-1);
            switch (var.scopeType) {
                case ScopeType::Module:
                    mFn->WriteOpcode(Opcode::StoreModuleVariable);
                    break;
                case ScopeType::Local:
                    mFn->WriteOpcode(Opcode::StoreLocalVariable);
                    break;
                case ScopeType::Upval:
                    mFn->WriteOpcode(Opcode::StoreUpvalue);
                    break;
                case ScopeType::Invalid:
                    // FIXME
                    break;
            }
            mFn->WriteShortOperand(var.index);
        }

        /**
         * 操作栈中压入常量
         * @param index
         */
        void EmitLoadConstant(HMInteger index) {
            STACK_CHANGE(1);
            mFn->WriteOpcode(Opcode::LoadConstant);
            mFn->WriteShortOperand(index);
        }

        void EmitAdd() {
            STACK_CHANGE(-1);
            mFn->WriteOpcode(Opcode::Add);
        }

        void EmitSub() {
            STACK_CHANGE(-1);
            mFn->WriteOpcode(Opcode::Sub);
        }

        void EmitDiv() {
            STACK_CHANGE(-1);
            mFn->WriteOpcode(Opcode::Div);
        }

        void EmitMul() {
            STACK_CHANGE(-1);
            mFn->WriteOpcode(Opcode::Mul);
        }

    };

    class Compiler {

    private:

    public:
        // 当前虚拟机
        VM *mVM;

        // 当前正在编译的模块
        HMModule *mCurModule;
        // 当前正在编译的函数
        CompileUnit *mCurCompileUnit;

        explicit Compiler(VM *mVM);

#ifdef HMDebug
        CompileUnit CreateCompileUnit(FunctionDebug *debug);
#else
        CompileUnit CreateCompileUnit();
#endif

    };

}

#endif //HYPERMIND_COMPILER_H
