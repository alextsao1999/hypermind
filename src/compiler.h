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
#define STACK_CHANGE(num) {mStackSlotNum += num;\
if (mStackSlotNum > mFn->maxStackSlotNum) mFn->maxStackSlotNum = mStackSlotNum;}

namespace hypermind {
    class VM;
    enum class CompileFlag {
        Null = 0,
        Assign = 1
    };
    enum class ScopeType {
        Invalid,
        Module,  // 模块变量
        Local, // 局部变量
        Upvalue
    };

    struct Upvalue {
        // 是否为外层直接局部变量
        bool isDirectOuterLocalVar;

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

    class CompileUnit {
        friend Compiler;
        friend ASTFunctionStmt;
    protected:
        // 所属虚拟机
        VM *mVM{nullptr};
        // 作用域深度
        HMInteger mScopeDepth{-1};

        Upvalue mUpvalues[MAX_UPVALUE_NUMBER]{};

        LocalVariable mLocalVariables[MAX_LOCAL_VAR_NUMBER]{};
        HMUINT32 mLocalVarNumber{0}; // 局部变量个数

        // 记录操作栈变化
        HMUINT32 mStackSlotNum{0};

    public:
        explicit CompileUnit(VM *mVm) : mVM(mVm) {};

        // 当前正在编译的函数
        HMFunction *mFn{nullptr};

        // 外层编译单元
        CompileUnit *mOuter{nullptr};

#ifdef HMDebug
        HMUINT32 mLine{0};
        HMUINT32 mColumn{0};
#endif

        inline void WriteByte(HMByte byte) {
            mFn->instructions.Append(byte);
#ifdef HMDebug
            mFn->debug->line.push_back(mLine);
#endif

        };
        /**
         * 写入操作码
         * @param opcode
         */
        void WriteOpcode(Opcode opcode) {
            WriteByte((HMByte) opcode);
        };
        /**
         * 写入 short 操作数 小端字节序
         * @param operand
         */
        void WriteShortOperand(int operand) {
            WriteByte(static_cast<HMByte>(operand & 0xff)); // 写入 低8位
            WriteByte(static_cast<HMByte>((operand >> 8) & 0xff)); // 写入 高8位
        };
        /**
         * 写入 Int 操作数 小端字节序
         * @param operand
         */
        void WriteIntOperand(int operand) {
            WriteByte(static_cast<HMByte>(operand & 0xff)); // 写入 0~7位
            WriteByte(static_cast<HMByte>((operand >> 8) & 0xff)); // 写入 8~15
            WriteByte(static_cast<HMByte>((operand >> 16) & 0xff)); // 写入 16~24
            WriteByte(static_cast<HMByte>((operand >> 24) & 0xff)); // 写入 25~32
        };

        /**
         * 当前作用域添加局部变量 返回索引
         * @param id
         * @return 返回索引
         */
        HMInteger AddLocalVariable(const Token &id) {
            if (mLocalVarNumber >= MAX_LOCAL_VAR_NUMBER) {
                // TODO 错误 变量数目大于最大局部变量
            }
            mLocalVariables[mLocalVarNumber].name = id.start;
            mLocalVariables[mLocalVarNumber].length = id.length;
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
            HMInteger index = FindLocal(id);
            if (index != -1) {
                // 此前已经存在该局部变量
                return index;
            }
            return AddLocalVariable(id);
        }

        /**
         * 定义变量   变量设置初始值
         * @param index
         */
        void DefineVariable(HMInteger index) {
            if (mScopeDepth == -1) {
                // 作用域为模块作用域
                //EmitStoreVariable(Variable(ScopeType::Module, index));
                //EmitPop();
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
                if (id.length == mLocalVariables[i].length &&
                    hm_memcmp(mLocalVariables[i].name, id.start, id.length) == 0) {
                    return i;
                }
            }
            return -1;
        }

        // 查找局部变量 或者 Upvalue
        Variable FindLocalOrUpvalue(const Token &id) {
            // 先寻找局部变量
            HMInteger index = FindLocal(id);
            if (index != -1) {
                return Variable(ScopeType::Local, index);
            }

            // 没找到局部变量 查找Upvalue
            index = FindUpvalue(id);
            if (index != -1) {
                return Variable(ScopeType::Upvalue, index);
            }

            return Variable(ScopeType::Invalid, 0);
        }

        /**
         * 查找变量
         * @param id
         * @return
         */
        Variable FindVariable(const Token &id) {
            Variable var = FindLocalOrUpvalue(id);
            if (var.scopeType == ScopeType::Invalid) {
                // TODO 查找模块变量
            }
            return var;
        };

        /**
         * 添加Upvalue
         * @param isDirectOuterLocalVar 是否为直接外层局部变量
         * @param index 索引
         * @return
         */
        HMInteger AddUpvalue(bool isDirectOuterLocalVar, HMInteger index) {
            // 如果存在就返回索引
            for (HMInteger i = 0; i < mFn->upvalueNum; ++i) {
                if (mUpvalues[i].index == index && mUpvalues[i].isDirectOuterLocalVar == isDirectOuterLocalVar) {
                    return i;
                }
            }
            // 不存在就添加
            mUpvalues[mFn->upvalueNum].index = index;
            mUpvalues[mFn->upvalueNum].isDirectOuterLocalVar = isDirectOuterLocalVar;
            return mFn->upvalueNum++;
        };

        HMInteger FindUpvalue(const Token &id) {
            if (mOuter == nullptr) {
                // 未找到
                return -1;
            }
            HMInteger index = mOuter->FindLocal(id);
            if (index != -1) {
                // 找到Upvalue 将局部变量Upvalue
                mOuter->SetLocalVariableUpvalue(index);
                return AddUpvalue(true, index);
            }
            // 递归查询添加
            index = mOuter->FindUpvalue(id);
            if (index != -1) {
                return AddUpvalue(false, index);
            }
            return -1;
        };

        /**
         * 设置变量为Upval
         * @param index
         */
        void SetLocalVariableUpvalue(HMInteger index) {
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
         * 丢弃作用域内的局部变量 返回丢弃个数
         */
        HMInteger DiscardLocalVariable() {
            HMInteger index = mLocalVarNumber - 1;
            while (index >= 0 && mLocalVariables[index].scopeDepth >= mScopeDepth) {
                if (mLocalVariables[index].isUpvalue)
                    EmitCloseUpvalue();
                else
                    EmitPop();
                index--;
            }
            return mLocalVarNumber - index - 1;
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

        void EmitCloseUpvalue() {
            STACK_CHANGE(-1);
            WriteOpcode(Opcode::CloseUpvalue);
        }

        void EmitPop() {
            STACK_CHANGE(-1);
            WriteOpcode(Opcode::Pop);
        }
        void EmitPushNull() {
            STACK_CHANGE(1);
            WriteOpcode(Opcode::PushNull);
        };
        void EmitPushTrue() {
            STACK_CHANGE(1);
            WriteOpcode(Opcode::PushTrue);
        };
        void EmitPushFalse() {
            STACK_CHANGE(1);
            WriteOpcode(Opcode::PushFalse);
        };

        /**
         * 弹出栈顶参数
         * @param argNum
         */
        void EmitCall(HMUINT32 methodIndex, HMUINT32 argNum) {
            STACK_CHANGE(-argNum);
            if (argNum <= 7) {
                WriteByte(static_cast<HMByte>((HMByte) Opcode::Call0 + argNum));
                WriteShortOperand(methodIndex);
            } else {
                WriteOpcode(Opcode::Call);
                WriteShortOperand(methodIndex); // 方法索引
                WriteShortOperand(argNum); // 实参数量
            }
        }

        void EmitReturn() {
            WriteOpcode(Opcode::Return);
        }

        /**
         * 操作栈中压入变量
         * @param var 变量信息(作用域和索引)
         */
        void EmitLoadVariable(const Variable &var) {
            STACK_CHANGE(1);
            switch (var.scopeType) {
                case ScopeType::Module:
                    WriteOpcode(Opcode::LoadModuleVariable);
                    break;
                case ScopeType::Local:
                    WriteOpcode(Opcode::LoadLocalVariable);
                    break;
                case ScopeType::Upvalue:
                    WriteOpcode(Opcode::LoadUpvalue);
                    break;
                case ScopeType::Invalid:
                    // FIXME
                    break;
            }
            WriteShortOperand(var.index);
        };

        /**
         * 储存变量
         * @param var
         */
        void EmitStoreVariable(const Variable &var) {
            switch (var.scopeType) {
                case ScopeType::Module:
                    WriteOpcode(Opcode::StoreModuleVariable);
                    break;
                case ScopeType::Local:
                    WriteOpcode(Opcode::StoreLocalVariable);
                    break;
                case ScopeType::Upvalue:
                    WriteOpcode(Opcode::StoreUpvalue);
                    break;
                case ScopeType::Invalid:
                    // FIXME
                    break;
            }
            WriteShortOperand(var.index);
        }

        /**
         * 操作栈中压入常量
         * @param index
         */
        void EmitLoadConstant(HMInteger index) {
            STACK_CHANGE(1);
            WriteOpcode(Opcode::LoadConstant);
            WriteShortOperand(index);
        }

        void EmitAdd() {
            STACK_CHANGE(-1);
            WriteOpcode(Opcode::Add);
        }
        void EmitSub() {
            STACK_CHANGE(-1);
            WriteOpcode(Opcode::Sub);
        }
        void EmitDiv() {
            STACK_CHANGE(-1);
            WriteOpcode(Opcode::Div);
        }
        void EmitMul() {
            STACK_CHANGE(-1);
            WriteOpcode(Opcode::Mul);
        }

        void EmitCreateClosure(HMInteger index) {
            STACK_CHANGE(1);
            WriteOpcode(Opcode::CreateClosure);
            WriteShortOperand(index);
        }

        void EmitEnd() {
            WriteOpcode(Opcode::End);
        }

    };

    class Compiler {

    private:

    public:
        // 当前虚拟机
        VM *mVM;

        // 当前正在编译的模块
        HMModule *mCurModule{};
        // 当前正在编译的函数
        CompileUnit *mCurCompileUnit{};

        explicit Compiler(VM *mVM) : mVM(mVM) {};

#ifdef HMDebug
        /**
         * 创建编译单元 并将其置为当前编译单元
         * @param debug 调试信息
         * @return
         */
        CompileUnit CreateCompileUnit(FunctionDebug *debug) {
            CompileUnit cu(mVM);
            cu.mFn = mVM->NewObject<HMFunction>(mCurModule);
            cu.mFn->debug = debug;
            cu.mOuter = mCurCompileUnit;
            return cu;
        }
#else
        CompileUnit CreateCompileUnit() {
        CompileUnit cu(mVM);
        cu.mFn = mVM->NewObject<HMFunction>(mCurModule);
        cu.mOuter = mCurCompileUnit;
        return cu;
    }
#endif
        /**
         * 离开编译单元 将创建编译单元之前的编译单元设为当前编译单元
         * @param cu
         */
        void LeaveCompileUnit(const CompileUnit &cu) {
            mCurCompileUnit = cu.mOuter;
        };

    };

}

#endif //HYPERMIND_COMPILER_H
