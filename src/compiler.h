//
// Created by 曹顺 on 2019/2/25.
//

#ifndef HYPERMIND_COMPILER_H
#define HYPERMIND_COMPILER_H

#include "hypermind.h"
#include "ast.h"
#include "obj/function.h"
#include "symbol.h"
#include "opcode.h"

// 操作栈改变 影响栈的深度
#define STACK_CHANGE(num) {mStackSlotNum += num;\
if (mStackSlotNum > mFn->maxStackSlotNum) mFn->maxStackSlotNum = mStackSlotNum;}

namespace hypermind {
    class VM;
    enum class CompileFlag {
        Null,
        Error,
        Static,
        Assign,
        Check,
        Setter,
        This,
        EmitCall,
    };
    enum class ScopeType {
        Invalid,
        Module,  // 模块变量
        Local, // 局部变量
        Upvalue,
        Class,
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
        Signature signature;
        HMInteger scopeDepth;  //局部变量作用域
        bool isUpvalue;
    };

    // 正在编译的类信息
    struct ClassInfo {
        SymbolTable fields;
        /*SymbolTable methods;*/
        ClassInfo *outer{nullptr};
    };

    enum class BlockType {
        None,
        For,
        While,
        If,
        Else,
    };

    struct BlockInfo {
        BlockType type; // 块类型
        HMUINT32 start{0};
        HMInteger scope{-1}; // 作用域深度
        struct {
            HMInteger condition{-1};
            HMInteger leave{-1};
        } lables;
        BlockInfo *outer{nullptr}; // 外部块
        explicit BlockInfo(BlockType type) : type(type) {};
    };

    class CompileUnit {
        friend Compiler;
        friend ASTFunctionStmt;
        friend ASTMethodStmt;
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
        explicit CompileUnit(VM *mVm, Compiler *compiler) : mVM(mVm), mCurCompiler(compiler) {};

        // 当前正在编译的函数
        HMFunction *mFn{nullptr};

        // 外层编译单元
        CompileUnit *mOuter{nullptr};
        Compiler *mCurCompiler{nullptr};
        ClassInfo *mCurClassInfo{nullptr};
        BlockInfo *mCurBlockInfo{nullptr};
        Buffer<HMUINT32> mLables;

#ifdef HMDebug
        HMUINT32 mLine{0};
        HMUINT32 mColumn{0};
#endif

        inline void WriteByte(HMByte byte) {
            mFn->instructions.push(&mVM->mGCHeap, byte);
#ifdef HMDebug
            mFn->debug->line.push(&mVM->mGCHeap, mLine);
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
        void WriteShortOperand(HMInteger operand) {
            WriteByte(static_cast<HMByte>(operand & 0xff)); // 写入 低8位
            WriteByte(static_cast<HMByte>((operand >> 8) & 0xff)); // 写入 高8位
        };
        /**
         * 写入 Int 操作数 小端字节序
         * @param operand
         */
        void WriteIntOperand(HMInteger operand) {
            WriteByte(static_cast<HMByte>(operand & 0xff)); // 写入 0~7位
            WriteByte(static_cast<HMByte>((operand >> 8) & 0xff)); // 写入 8~15
            WriteByte(static_cast<HMByte>((operand >> 16) & 0xff)); // 写入 16~24
            WriteByte(static_cast<HMByte>((operand >> 24) & 0xff)); // 写入 25~32
        };

        void RewriteShortOperand(HMInteger index, HMInteger operand) {
            mFn->instructions[index] = static_cast<HMByte>(operand & 0xff); // 写入 低8位
            mFn->instructions[index + 1] = static_cast<HMByte>((operand >> 8) & 0xff); // 写入 高8位
        }

        inline void EnterBlock(BlockInfo *blockInfo) {
            if (blockInfo->type == BlockType::While) {
                EmitEnterBlock();
            }
            blockInfo->outer = mCurBlockInfo;
            blockInfo->start = mFn->instructions.count;
            blockInfo->scope = mScopeDepth;
            mCurBlockInfo = blockInfo;
        }

        inline void LeaveBlock() {
            if (mCurBlockInfo != nullptr) {
                if (mCurBlockInfo->type == BlockType::While) {
                    EmitLeaveBlock();
                }
                mCurBlockInfo = mCurBlockInfo->outer;
            }
        }

        inline void FixLable(BlockInfo *block) {
            if (block->outer == nullptr) {
                HMByte *code = mFn->instructions.data;
                for (int i = block->start; i < mFn->instructions.count; ++i) {
                    auto opcode = (Opcode) code[i];
                    switch (opcode) {
                        case Opcode::LoadConstant:
                        case Opcode::LoadLocalVariable:
                        case Opcode::StoreLocalVariable:
                        case Opcode::LoadModuleVariable:
                        case Opcode::StoreModuleVariable:
                        case Opcode::LoadUpvalue:
                        case Opcode::StoreUpvalue:
                        case Opcode::LoadThisField:
                        case Opcode::StoreThisField:
                        case Opcode::LoadField:
                        case Opcode::StoreField:
                        case Opcode::Call0:
                        case Opcode::Call1:
                        case Opcode::Call2:
                        case Opcode::Call3:
                        case Opcode::Call4:
                        case Opcode::Call5:
                        case Opcode::Call6:
                        case Opcode::Call7:
                        case Opcode::Super0:
                        case Opcode::Super1:
                        case Opcode::Super2:
                        case Opcode::Super3:
                        case Opcode::Super4:
                        case Opcode::Super5:
                        case Opcode::Super6:
                        case Opcode::Super7:
                        case Opcode::CreateClass:
                        case Opcode::BindInstanceMethod:
                        case Opcode::BindStaticMethod:
                            i += 2;
                            break;
                        case Opcode::Call:
                        case Opcode::Super:
                        case Opcode::BindFieldMethod:
                            i += 4;
                            break;
                        case Opcode::Loop:
                        {
                            HMInteger index = code[i + 1] | (code[i + 2] << 8);
                            RewriteShortOperand(i + 1, i + 3 - mLables[index]);
                            i += 2;
                        }
                            break;
                        case Opcode::Jump:
                        case Opcode::JumpIfFalse:
                        case Opcode::JumpIfTrue:
                        {
                            HMInteger index = code[i + 1] | (code[i + 2] << 8);
                            RewriteShortOperand(i + 1, mLables[index] - i - 3);
                            i += 2;
                        }
                            break;
                        default:
                            break;
                    }
                }

            }

        }

        HMInteger CreateLable() {
            return mLables.push(&mVM->mGCHeap, 0);
        }

        void SetLable(HMInteger lable) {
            mLables[lable] = mFn->instructions.count;
        }

        /**
         * 当前作用域添加局部变量 返回索引
         * @param id
         * @return 返回索引
         */
        HMInteger AddLocalVariable(Signature signature) {
            if (mLocalVarNumber >= MAX_LOCAL_VAR_NUMBER) {
                // TODO 错误 变量数目大于最大局部变量
            }
            mLocalVariables[mLocalVarNumber].signature = signature;
            mLocalVariables[mLocalVarNumber].scopeDepth = mScopeDepth;
            mLocalVariables[mLocalVarNumber].isUpvalue = false;
            if (mLocalVarNumber + 1 > mFn->maxLocalVarNum) {
                mFn->maxLocalVarNum = mLocalVarNumber + 1;
            }
            return mLocalVarNumber++;
        };

        /**
         * 根据作用域声明变量/参数
         * @return
         */
        HMInteger DeclareVariable(Signature signature);

        /**
         * 定义变量   变量设置初始值
         * @param index
         */
        void DefineVariable(HMInteger index) {
            if (mScopeDepth == -1) {
                // 作用域为模块作用域
                EmitStoreVariable(Variable(ScopeType::Module, index));
                EmitPop();
            } else {
//                EmitStoreVariable(Variable(ScopeType::Local, index));
            }

            // 不是模块作用域 不用管
        }

        /**
         * 查找局部变量
         * @param id
         * @return
         */
        HMInteger FindLocal(Signature signature) {
            for (HMUINT32 i = 0; i < mLocalVarNumber; ++i) {
                if (signature == mLocalVariables[i].signature) {
                    return i;
                }
            }
            return -1;
        }

        // 查找局部变量 或者 Upvalue
        Variable FindLocalOrUpvalue(Signature signature) {
            // 先寻找局部变量
            HMInteger index = FindLocal(signature);
            if (index != -1) {
                return Variable(ScopeType::Local, index);
            }

            // 没找到局部变量 查找Upvalue
            index = FindUpvalue(signature);
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
        Variable FindVariable(Signature signature);

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

        HMInteger FindUpvalue(Signature signature) {
            if (mOuter == nullptr) {
                // 未找到
                return -1;
            }
            HMInteger index = mOuter->FindLocal(signature);
            if (index != -1) {
                // 找到Upvalue 将局部变量Upvalue
                mOuter->SetLocalVariableUpvalue(index);
                return AddUpvalue(true, index);
            }
            // 递归查询添加
            index = mOuter->FindUpvalue(signature);
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
            return mFn->constants.push(&mVM->mGCHeap, value);
        };

        /**
         * 添加符号
         * @param signature
         * @return
         */
        HMInteger AddSymbol(Signature signature) {
            return mFn->symbols.Add(&mVM->mGCHeap, signature);
        }

        void StoreUpvalue() {
            mFn->upvalues = mVM->Allocate<Upvalue>(mFn->upvalueNum);
            memcpy(mFn->upvalues, mUpvalues, sizeof(Upvalue) * mFn->upvalueNum);
        }

        // 加载this的upvalue
        void LoadThis();

        void LoadModuleVar(Signature signature);

        void EmitEnterBlock() {
            WriteOpcode(Opcode::EnterBlock);
        }
        void EmitLeaveBlock() {
            WriteOpcode(Opcode::LeaveBlock);
        }

        void EmitJumpIfTrue(HMInteger lable) {
            STACK_CHANGE(-1);
            WriteOpcode(Opcode::JumpIfTrue);
            WriteShortOperand(lable);
        }
        void EmitJumpIfFalse(HMInteger lable) {
            STACK_CHANGE(-1);
            WriteOpcode(Opcode::JumpIfFalse);
            WriteShortOperand(lable);
        }
        void EmitJump(HMInteger lable) {
            WriteOpcode(Opcode::Jump);
            WriteShortOperand(lable);
        }
        void EmitLoop(HMInteger lable) {
            WriteOpcode(Opcode::Loop);
            WriteShortOperand(lable);
        }

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

        void EmitBindMethod(HMInteger index, HMBool isStatic) {
            STACK_CHANGE(-1);
            WriteOpcode(isStatic ? Opcode::BindStaticMethod : Opcode::BindInstanceMethod);
            WriteShortOperand(index);
        }

        void EmitBindMethod(Signature signature, HMBool isStatic) {
            STACK_CHANGE(-1);
            WriteOpcode(isStatic ? Opcode::BindStaticMethod : Opcode::BindInstanceMethod);
            HMInteger index = mVM->mAllMethods.EnsureFind(&mVM->mGCHeap, signature);
            WriteShortOperand(index);
        }

        void EmitFieldMethod(Signature signature, HMInteger field) {
            WriteOpcode(Opcode::BindFieldMethod);
            HMInteger index = mVM->mAllMethods.EnsureFind(&mVM->mGCHeap, signature);
            WriteShortOperand(index);
            WriteShortOperand(field);
        }

        /**
         * 弹出栈顶参数
         * @param argNum
         */
        void EmitCall(HMInteger methodIndex, HMInteger argNum) {
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

        void EmitCall(Signature signature, HMInteger argNum) {
            HMInteger index = mVM->mAllMethods.EnsureFind(&mVM->mGCHeap, signature);
            EmitCall(static_cast<HMUINT32>(index), argNum);
        };

        void EmitReturn() {
            STACK_CHANGE(-1);
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
                case ScopeType::Class:
                    WriteOpcode(Opcode::LoadThisField);
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
                case ScopeType::Class:
                    WriteOpcode(Opcode::StoreThisField);
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

        void EmitLoadField(HMInteger index) {
            STACK_CHANGE(1);
            WriteOpcode(Opcode::LoadField);
            WriteShortOperand(index);
        }

        void EmitStoreField(HMInteger index) {
            WriteOpcode(Opcode::StoreField);
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

        void EmitCreateInstance() {
            WriteOpcode(Opcode::CreateInstance);
        }

        void EmitCreateClass(HMInteger fieldNumber) {
            STACK_CHANGE(1);
            WriteOpcode(Opcode::CreateClass);
            WriteShortOperand(fieldNumber);
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

        // 当前编译的类信息
        ClassInfo *mCurClassInfo{nullptr};

        explicit Compiler(VM *mVM) : mVM(mVM) {};

#ifdef HMDebug
        /**
         * 创建编译单元 并将其置为当前编译单元
         * @param debug 调试信息
         * @return
         */
        CompileUnit CreateCompileUnit(FunctionDebug *debug) {
            CompileUnit cu(mVM, this);
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
        void LeaveCompileUnit(CompileUnit &cu, HMBool isConstrutor) {
            if (isConstrutor) {
                cu.EmitLoadVariable(Variable(ScopeType::Local, 0));
            } else {
                cu.EmitPushNull();
            }
            cu.EmitReturn();
            cu.mLables.clear(&mVM->mGCHeap);
            mCurCompileUnit = cu.mOuter;

        };

    };

}

#endif //HYPERMIND_COMPILER_H
