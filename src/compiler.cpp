//
// Created by 曹顺 on 2019/2/25.
//

#include "compiler.h"

namespace hypermind {
    // 编译语法块
    AST_COMPILE(ASTBlock) {
        for (auto &stmt : stmts)
            stmt->compile(compiler, false);
    }

    // 编译二元表达式
    AST_COMPILE(ASTBinary) {
        mLHS->compile(compiler, false);
        mRHS->compile(compiler, false);

        // 根据Op 编译相应的Opcode
        switch (mOp.mType) {
            case TokenType::Add:
                break;
            case TokenType::Sub:
                break;
            case TokenType::Mul:
                break;
            case TokenType::Div:
                break;
            case TokenType::Increase:
                break;
            case TokenType::Decrease:
                break;
            case TokenType::Assign:

                break;
            case TokenType::AddAssign:
                break;
            case TokenType::SubAssign:
                break;
            case TokenType::MulAssign:
                break;
            case TokenType::DivAssign:
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
        HMInteger idx = compiler->mCurCompileUnit->AddConstant(mValue);
        compiler->mCurCompileUnit->EmitLoadConstant(idx);
    }

    // 编译变量
    AST_COMPILE(ASTVariable) {
        Variable var = compiler->mCurCompileUnit->FindVariable(mVar);
        compiler->mCurCompileUnit->EmitLoadVariable(var);

    }

    // 编译IF
    AST_COMPILE(ASTIfStmt) {

    }

    // 编译While
    AST_COMPILE(ASTWhileStmt) {

    }

    // 编译Continue
    AST_COMPILE(ASTContinueStmt) {

    }

    // 编译Break
    AST_COMPILE(ASTBreakStmt) {

    }

    // 编译Return
    AST_COMPILE(ASTReturnStmt) {

    }

    // 编译List
    AST_COMPILE(ASTList) {
        for (auto &element : elements)
            element->compile(compiler, false);
    }

    /**
     * 编译变量声明
     * 声明变量的时候 如果初始值为空 则压入Null值 如果 初始值不为空 则 压入初始值
     * @param cu
     */
    AST_COMPILE(ASTVarStmt) {
        if (mValue == nullptr) {
            compiler->mCurCompileUnit->EmitPushNull();
        } else {
            mValue->compile(compiler, false);
        }
        compiler->mCurCompileUnit->DeclareLocalVariable(mIdentifier);

    }

    // 编译函数
    AST_COMPILE(ASTFunctionStmt) {
        if (compiler->mCurCompileUnit->mFn != nullptr) {
            // TODO
        }
        // 在当前虚拟机中创建一个函数对象
        compiler->mCurCompileUnit->mFn =
                compiler->mCurCompileUnit->mVM->NewObject<HMFunction>(compiler->mCurModule);
        // 进入作用域
        compiler->mCurCompileUnit->EnterScope();
        mParams->compile(compiler, false); // 声明变量
        mBody->compile(compiler, false); // 编译函数实体
        // 离开作用域
        compiler->mCurCompileUnit->LeaveScope();
#ifdef DEBUG
        compiler->mCurCompileUnit->mFn->debug =
                compiler->mVM->New<FunctionDebug>(String(mName.mStart, mName.mLength));
#endif

    }

    // 编译类
    AST_COMPILE(ASTClassStmt) {

    }

    CompileUnit::CompileUnit(VM *mVm) : mVM(mVm) {

    }

    Compiler::Compiler(VM *mVM) : mVM(mVM) {

    }
}