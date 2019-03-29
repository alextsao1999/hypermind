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
            stmt->compile(compiler, false);
    }

    // 编译二元表达式
    AST_COMPILE(ASTBinary) {
        AST_ENTER();
        // Σ(っ °Д °;)っ 这是个异类
        if (op == TokenType::Assign) {
            rhs->compile(compiler, false);
            lhs->compile(compiler, true);
            return;
        }
        lhs->compile(compiler, false);
        rhs->compile(compiler, false);
        // 根据Op 编译相应的Opcode
        switch (op) {
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
                lhs->compile(compiler, true);
                break;
            case TokenType::SubAssign:
                compiler->mCurCompileUnit->EmitSub();
                lhs->compile(compiler, true);
                break;
            case TokenType::MulAssign:
                compiler->mCurCompileUnit->EmitMul();
                lhs->compile(compiler, true);
                break;
            case TokenType::DivAssign:
                compiler->mCurCompileUnit->EmitDiv();
                lhs->compile(compiler, true);
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
        if (isAssign)
            compiler->mCurCompileUnit->EmitStoreVariable(variable);
        else
            compiler->mCurCompileUnit->EmitLoadVariable(variable);
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
    }

    // 编译List
    AST_COMPILE(ASTList) {
        AST_ENTER();
        for (auto &element : elements)
            element->compile(compiler, false);
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
            value->compile(compiler, false);
        }
        HMInteger index = compiler->mCurCompileUnit->AddVariable(identifier);
        // 局部变量并没有什么变化
        //  如果是模块变量的话 会把局部变量值弹出 存到模块变量中
        compiler->mCurCompileUnit->DefineVariable(index);
    }

    AST_COMPILE(ASTParamStmt) {
        AST_ENTER();
        // 声明参数
        compiler->mCurCompileUnit->AddVariable(identifier);
    }

    // 编译函数
    AST_COMPILE(ASTFunctionStmt) {
        AST_ENTER();
        // 创建编译单元
#ifdef HMDebug
        //  附上调试信息
        CompileUnit cu = compiler->CreateCompileUnit(new FunctionDebug(String(name.start, name.length)));
#else
        CompileUnit cu = compiler->CreateCompileUnit();
#endif
        compiler->mCurCompileUnit = &cu;
        if (cu.mOuter != nullptr) {
            // TODO 如果为 函数 则 添加Null 为 方法就添加This
            cu.AddLocalVariable(Token(TokenType::Identifier, nullptr, 0, 0));
            // 进入作用域
            cu.EnterScope(); // 直接就编译了 不需要离开作用域
        }
        cu.mFn->maxStackSlotNum = cu.mStackSlotNum;

        params->compile(compiler, false); // 编译参数声明
        body->compile(compiler, false); // 编译函数实体
        compiler->LeaveCompileUnit(cu);
        cu.EmitEnd();
        cu.mFn->upvalues = compiler->mVM->Allocate<Upvalue>(cu.mFn->upvalueNum);
        memcpy(cu.mFn->upvalues, cu.mUpvalues, sizeof(Upvalue) * cu.mFn->upvalueNum);
        HMInteger index = cu.mOuter->AddConstant(Value(cu.mFn));
        cu.mOuter->EmitCreateClosure(index);

    }

    // 编译类
    AST_COMPILE(ASTClassStmt) {
        AST_ENTER();

    }


}