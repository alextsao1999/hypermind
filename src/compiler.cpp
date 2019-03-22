//
// Created by 曹顺 on 2019/2/25.
//

#include "compiler.h"

namespace hypermind {
    // 编译语法块
    AST_COMPILE(ASTBlock) {
        for (auto &stmt : stmts)
            stmt->compile(cu, false);
    }

    // 编译二元表达式
    AST_COMPILE(ASTBinary) {
        mLHS->compile(cu, false);
        mRHS->compile(cu, false);

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
            case TokenType::XorAssign:break;
            case TokenType::Arrow:break;
            case TokenType::Not:break;
            case TokenType::Equal:break;
            case TokenType::NotEqual:break;
            case TokenType::Greater:break;
            case TokenType::Less:break;
            case TokenType::GreaterEqual:break;
            case TokenType::LessEqual:break;
            case TokenType::Or:break;
            case TokenType::LogicOr:break;
            case TokenType::And:break;
            case TokenType::LogicAnd:break;
            case TokenType::Mod:break;
            default:
                break;
        }

    }

    // 编译字面量
    AST_COMPILE(ASTLiteral) {
        HMUINT32 idx = cu->AddConstant(mValue);
        cu->EmitLoadConstant(idx);
    }

    // 编译变量
    AST_COMPILE(ASTVariable) {
        Variable var = cu->FindVariable(mVar);
        cu->EmitLoadVariable(var);

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
            element->compile(cu, false);
    }

    /**
     * 编译变量声明
     * 声明变量的时候 如果初始值为空 则压入Null值 如果 初始值不为空 则 压入初始值
     * @param cu
     */
    AST_COMPILE(ASTVarStmt) {
        if (mValue == nullptr) {
            cu->EmitPushNull();
        } else {
            mValue->compile(cu, false);
        }
        cu->DeclareLocalVariable(mIdentifier);

    }

    // 编译函数
    AST_COMPILE(ASTFunctionStmt) {
        if (cu->mFn != nullptr) {
            // TODO 错误 : 已经存在正在编译的函数
        }
        // 在当前虚拟机中创建一个函数对象
        // TODO 当前模块变量为nullptr 编译类的时候需要
        cu->mFn = cu->mVM->New<HMFunction>(cu->mVM, nullptr);

#ifdef DEBUG
        cu->mFn->debug = cu->mVM->New<FunctionDebug>(String(mName.mStart, mName.mLength));
#endif
        mParams->compile(cu, false); // 声明变量
        mBody->compile(cu, false); // 编译函数实体
    }

    // 编译类
    AST_COMPILE(ASTClassStmt) {

    }

    CompileUnit::CompileUnit(VM *mVm) : mVM(mVm) {

    }
    
}