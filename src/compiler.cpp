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

        // Σ(っ °Д °;)っ 这是个异类
        if (mOp.mType == TokenType::Assign) {
            mRHS->compile(compiler, false);
            mLHS->compile(compiler, true);
            return;
        }
        mLHS->compile(compiler, false);
        mRHS->compile(compiler, false);
        // 根据Op 编译相应的Opcode
        switch (mOp.mType) {
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
                mLHS->compile(compiler, true);
                break;
            case TokenType::SubAssign:
                compiler->mCurCompileUnit->EmitSub();
                mLHS->compile(compiler, true);
                break;
            case TokenType::MulAssign:
                compiler->mCurCompileUnit->EmitMul();
                mLHS->compile(compiler, true);
                break;
            case TokenType::DivAssign:
                compiler->mCurCompileUnit->EmitDiv();
                mLHS->compile(compiler, true);
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
        switch (mValue.type) {
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
                HMInteger idx = compiler->mCurCompileUnit->AddConstant(mValue);
                compiler->mCurCompileUnit->EmitLoadConstant(idx);
                break;
        }
    }

    // 编译变量
    AST_COMPILE(ASTVariable) {
        Variable var = compiler->mCurCompileUnit->FindVariable(mVar);
        if (isAssign)
            compiler->mCurCompileUnit->EmitStoreVariable(var);
        else
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
        HMInteger index = compiler->mCurCompileUnit->DeclareVariable(mIdentifier);
        // 局部变量并没有什么变化
        //  如果是模块变量的话 会把局部变量值弹出 存到模块变量中
        compiler->mCurCompileUnit->DefineVariable(index);
    }

    AST_COMPILE(ASTParamStmt) {
        // 声明参数
        compiler->mCurCompileUnit->DeclareVariable(mIdentifier);
    }

    // 编译函数
    AST_COMPILE(ASTFunctionStmt) {
        // 创建编译单元
#ifdef HMDebug
        //  附上调试信息
        //        compiler->mCurCompileUnit->mFn->debug =
        //                compiler->mVM->New<FunctionDebug>(String(mName.mStart, mName.mLength));
        CompileUnit cu = compiler->CreateCompileUnit(new FunctionDebug(String(mName.mStart, mName.mLength)));
#else
        CompileUnit cu = compiler->CreateCompileUnit();
#endif
        compiler->mCurCompileUnit = &cu;
        // 进入作用域
        cu.EnterScope();
        mParams->compile(compiler, false); // 声明变量
        mBody->compile(compiler, false); // 编译函数实体
        // 离开作用域
        cu.LeaveScope();
        compiler->mCurCompileUnit = cu.mOuter;

    }

    // 编译类
    AST_COMPILE(ASTClassStmt) {

    }

    CompileUnit::CompileUnit(VM *mVm) : mVM(mVm) {
    }

    Compiler::Compiler(VM *mVM) : mVM(mVM) {
    }

#ifdef HMDebug
    CompileUnit Compiler::CreateCompileUnit(FunctionDebug *debug) {
        CompileUnit cu(mVM);
        cu.mFn = mVM->NewObject<HMFunction>(mCurModule);
        cu.mFn->debug = debug;
        cu.mOuter = mCurCompileUnit;
        return cu;
    }
#else
    CompileUnit Compiler::CreateCompileUnit() {
        CompileUnit cu(mVM);
        cu.mFn = mVM->NewObject<HMFunction>(mCurModule);
        cu.mOuter = mCurCompileUnit;
        return cu;
    }
#endif

}