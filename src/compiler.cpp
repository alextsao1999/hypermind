//
// Created by 曹顺 on 2019/2/25.
//

#include "compiler.h"

#define AST_ENTER() {compiler->mCurCompileUnit->mLine = line; \
compiler->mCurCompileUnit->mColumn = column;}

namespace hypermind {
    // 编译语法块
    AST_COMPILE(ASTBlock) {
        AST_ENTER();
        for (auto &stmt : stmts)
            stmt->compile(compiler);
    }

    // 编译二元表达式
    AST_COMPILE(ASTBinary) {
        AST_ENTER();
        // Σ(っ °Д °;)っ 这是个异类
        if (op.type == TokenType::Assign) {
            rhs->compile(compiler, CompileFlag::Null);
            lhs->compile(compiler, CompileFlag::Assign);
            return;
        }
        lhs->compile(compiler);
        rhs->compile(compiler);
        // 根据Op 编译相应的Opcode
        switch (op.type) {
            case TokenType::Add:
                compiler->mCurCompileUnit->EmitAdd();
                break;
            case TokenType::Sub:
                compiler->mCurCompileUnit->EmitSub();
                break;
            case TokenType::Mul:
                compiler->mCurCompileUnit->EmitMul();
                break;
            case TokenType::Div:
                compiler->mCurCompileUnit->EmitDiv();
                break;
            case TokenType::Increase:
                break;
            case TokenType::Decrease:
                break;
            case TokenType::Assign:
                break;
            case TokenType::AddAssign:
                compiler->mCurCompileUnit->EmitAdd();
                lhs->compile(compiler);
                break;
            case TokenType::SubAssign:
                compiler->mCurCompileUnit->EmitSub();
                lhs->compile(compiler);
                break;
            case TokenType::MulAssign:
                compiler->mCurCompileUnit->EmitMul();
                lhs->compile(compiler);
                break;
            case TokenType::DivAssign:
                compiler->mCurCompileUnit->EmitDiv();
                lhs->compile(compiler);
                break;
            case TokenType::ModAssign:
                break;
            case TokenType::AndAssign:
                break;
            case TokenType::OrAssign:
                break;
            case TokenType::XorAssign:
                break;
            case TokenType::Arrow:
                break;
            case TokenType::Not:
                break;
            case TokenType::Equal:
                break;
            case TokenType::NotEqual:
                break;
            case TokenType::Greater:
                break;
            case TokenType::Less:
                break;
            case TokenType::GreaterEqual:
                break;
            case TokenType::LessEqual:
                break;
            case TokenType::Or:
                break;
            case TokenType::LogicOr:
                break;
            case TokenType::And:
                break;
            case TokenType::LogicAnd:
                break;
            case TokenType::Mod:
                break;
            default:
                break;
        }

    }

    // 编译字面量
    AST_COMPILE(ASTLiteral) {
        AST_ENTER();
        switch (value.type) {
            case ValueType::Undefined:
                break;
            case ValueType::Null:
                compiler->mCurCompileUnit->EmitPushNull();
                break;
            case ValueType::True:
                compiler->mCurCompileUnit->EmitPushTrue();
                break;
            case ValueType::False:
                compiler->mCurCompileUnit->EmitPushFalse();
                break;
            case ValueType::Integer:
            case ValueType::Float:
            case ValueType::Object:
                HMInteger idx = compiler->mCurCompileUnit->AddConstant(value);
                compiler->mCurCompileUnit->EmitLoadConstant(idx);
                break;
        }
    }

    // 编译变量
    AST_COMPILE(ASTVariable) {
        AST_ENTER();
        Variable variable = compiler->mCurCompileUnit->FindVariable(var);
        if (variable.scopeType == ScopeType::Invalid) {
            if (compiler->mCurCompileUnit->mCurClassInfo != nullptr) {
                // 查找Class Field
                variable.index = compiler->mCurCompileUnit->mCurClassInfo->fields.Find(var);
                if (variable.index == -1) {
                    // TODO Error 域不存在
                }
                compiler->mCurCompileUnit->EmitLoadThisField(variable.index);
            } else {
                // TODO Error 变量不存在
            }
            return;
        }

        if (flag == CompileFlag::Assign) {
            compiler->mCurCompileUnit->EmitStoreVariable(variable);
        } else {
            compiler->mCurCompileUnit->EmitLoadVariable(variable);
        }
    }

    // 编译IF
    AST_COMPILE(ASTIfStmt) {
        AST_ENTER();

    }

    // 编译While
    AST_COMPILE(ASTWhileStmt) {
        AST_ENTER();
    }

    // 编译Continue
    AST_COMPILE(ASTContinueStmt) {
        AST_ENTER();
    }

    // 编译Break
    AST_COMPILE(ASTBreakStmt) {
        AST_ENTER();
    }

    // 编译Return
    AST_COMPILE(ASTReturnStmt) {
        AST_ENTER();
        if (retvalue == nullptr) {
            compiler->mCurCompileUnit->EmitPushNull();
        } else {
            retvalue->compile(compiler);
        }
        compiler->mCurCompileUnit->EmitReturn();
    }

    // 编译List
    AST_COMPILE(ASTList) {
        AST_ENTER();
        for (auto &element : elements)
            element->compile(compiler);
    }

    /**
     * 编译变量声明
     * 声明变量的时候 如果初始值为空 则压入Null值 如果 初始值不为空 则 压入初始值
     * @param cu
     */
    AST_COMPILE(ASTVarStmt) {
        AST_ENTER();
        if (value == nullptr) {
            compiler->mCurCompileUnit->EmitPushNull();
        } else {
            value->compile(compiler);
        }
        HMInteger index = compiler->mCurCompileUnit->DeclareVariable(identifier);
        // 局部变量并没有什么变化
        //  如果是模块变量的话 会把局部变量值弹出 存到模块变量中
        compiler->mCurCompileUnit->DefineVariable(index);
    }

    AST_COMPILE(ASTParamStmt) {
        AST_ENTER();
        // 声明参数
        compiler->mCurCompileUnit->AddLocalVariable(identifier);
    }

    // 编译函数
    AST_COMPILE(ASTFunctionStmt) {
        AST_ENTER();
        HMInteger funIdx = compiler->mCurCompileUnit->DeclareVariable(name);
        // 创建编译单元
#ifdef HMDebug
        //  附上调试信息
        CompileUnit cu = compiler->CreateCompileUnit(new FunctionDebug(name));
#else
        CompileUnit cu = compiler->CreateCompileUnit();
#endif
        compiler->mCurCompileUnit = &cu;
        cu.AddLocalVariable("this");
        // 进入作用域
        cu.EnterScope(); // 直接就编译了 不需要离开作用域
        cu.mFn->maxStackSlotNum = cu.mStackSlotNum = cu.mLocalVarNumber;
        params->compile(compiler); // 编译参数声明
        cu.mFn->argNum = cu.mLocalVarNumber - 1;
        body->compile(compiler); // 编译函数实体
        compiler->LeaveCompileUnit(cu);
        cu.EmitEnd();
        // 记录函数上值 创建闭包的时候用
        cu.mFn->upvalues = compiler->mVM->Allocate<Upvalue>(cu.mFn->upvalueNum);
        memcpy(cu.mFn->upvalues, cu.mUpvalues, sizeof(Upvalue) * cu.mFn->upvalueNum);
        HMInteger index = cu.mOuter->AddConstant(Value(cu.mFn));
        cu.mOuter->EmitCreateClosure(index);
        cu.mOuter->DefineVariable(funIdx);

    }

    // 编译类
    AST_COMPILE(ASTClassStmt) {
        AST_ENTER();
        ClassInfo classInfo;
        compiler->mCurCompileUnit->mCurClassInfo = &classInfo;
        for (auto &field : fields) {
            classInfo.fields.Add(&compiler->mVM->mGCHeap, Signature(field->identifier));
        }
        if (super.type == TokenType::End) {
            compiler->mCurCompileUnit->LoadModuleVar("object");
        } else {
            compiler->mCurCompileUnit->LoadModuleVar(super);
        }
        compiler->mCurCompileUnit->EmitCreateClass(classInfo.fields.mSymbols.count);

        for (auto &method : methods) {
            method->compile(compiler, CompileFlag::Null);
        }


    }

    AST_COMPILE(ASTMethodStmt) {
        AST_ENTER();
#ifdef HMDebug
        //  附上调试信息
        CompileUnit cu = compiler->CreateCompileUnit(new FunctionDebug(name));
#else
        CompileUnit cu = compiler->CreateCompileUnit();
#endif
        compiler->mCurCompileUnit = &cu;
        cu.EnterScope();
        cu.mFn->maxStackSlotNum = cu.mStackSlotNum = cu.mLocalVarNumber;
        params->compile(compiler); // 编译参数声明
        cu.mFn->argNum = cu.mLocalVarNumber - 1;
        body->compile(compiler); // 编译函数实体
        compiler->LeaveCompileUnit(cu);
        cu.EmitEnd();
        // 记录函数上值 创建闭包的时候用
        cu.mFn->upvalues = compiler->mVM->Allocate<Upvalue>(cu.mFn->upvalueNum);
        memcpy(cu.mFn->upvalues, cu.mUpvalues, sizeof(Upvalue) * cu.mFn->upvalueNum);
        HMInteger index = cu.mOuter->AddConstant(Value(cu.mFn));
        cu.mOuter->EmitCreateClosure(index);
        cu.mOuter->EmitInstanceMethod(0);


    }

    AST_COMPILE(ASTNegativeExpr) {
        AST_ENTER();

    }

    AST_COMPILE(ASTNotExpr) {
        AST_ENTER();

    }

    AST_COMPILE(ASTArgPostfix) {
        AST_ENTER();
        expr->compile(compiler);
        args->compile(compiler, CompileFlag::Null);
        compiler->mCurCompileUnit->EmitCall(0, args->elements.size());

    }

    AST_COMPILE(ASTDotPostfix) {
        AST_ENTER();

    }


    HMInteger CompileUnit::DeclareVariable(Signature signature) {
        if (mScopeDepth == -1) {
            mCurCompiler->mCurModule->varNames.Add(&mVM->mGCHeap, signature);
            return mCurCompiler->mCurModule->varValues.append(&mVM->mGCHeap, Value());
        }

        HMInteger index = FindLocal(signature);
        if (index != -1) {
            // 此前已经存在该局部变量
            return index;
        }
        return AddLocalVariable(signature);
    }

    Variable CompileUnit::FindVariable(Signature signature) {
        Variable var = FindLocalOrUpvalue(signature);
        if (var.scopeType == ScopeType::Invalid) {
            // TODO 查找模块变量
            HMInteger index = mCurCompiler->mCurModule->varNames.Find(signature);
            if (index != -1) {
                return Variable(ScopeType::Module, index);
            }
        }
        return var;
    }
}