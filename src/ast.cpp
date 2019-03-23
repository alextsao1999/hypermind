//
// Created by 曹顺 on 2019/3/1.
//

#include "ast.h"

namespace hypermind {
    // 语法块Dump
    AST_DUMP(ASTBlock) {
        os << _HM_C("  {  ") << std::endl;
        for (auto &stmt : stmts) {
            os << _HM_C("    ");
            stmt->dump(os);
            os << std::endl;
        }
        os << _HM_C(" } ");

    }

    // 二元表达式Dump
    AST_DUMP(ASTBinary) {
        os << _HM_C(" { (binary)  ");
        mLHS->dump(os);
        mOp.dump(os);
        mRHS->dump(os);
        os << _HM_C("  } ") ;
    }

    // 字面量Dump
    AST_DUMP(ASTLiteral) {
        mValue.dump(os);
    }

    AST_DUMP(ASTVariable) {
        // 字面量Dump
        mVar.dump(os);

    }

    AST_DUMP(ASTIfStmt) {
        os << _HM_C("if  condition : ");
        mCondition->dump(os);
        os << std::endl;
        mThen->dump(os);
        if (mElse != nullptr) {
            os << _HM_C(" else ");
            mElse->dump(os);
        }

    }

    AST_DUMP(ASTWhileStmt) {
        os << _HM_C("while  condition : ");
        mCondition->dump(os);
        os << std::endl;
        mBlock->dump(os);
    }

    AST_DUMP(ASTContinueStmt) {
        os << _HM_C("continue");
    }

    AST_DUMP(ASTBreakStmt) {
        os << _HM_C("break");
    }
    AST_DUMP(ASTReturnStmt) {
        os << _HM_C("return ");
        mRetval->dump(os);

    }

    AST_DUMP(ASTVarStmt) {
        os << _HM_C(" define var : ") ;
        mIdentifier.dump(os);
        if (mValue != nullptr) {
            os << _HM_C(" = ");
            mValue->dump(os);
        }
        os << _HM_C("  ") << std::endl;
    }

    AST_DUMP(ASTList) {
        os << _HM_C("  ( ");
        for (auto &element : elements) {
            os << _HM_C("   ");
            element->dump(os);
            os << _HM_C(", ");
        }
        os << _HM_C(" ) ");
    }

    AST_DUMP(ASTFunctionStmt) {
        os << _HM_C(" func  name : ");
        mName.dump(os);
        mParams->dump(os);
        mBody->dump(os);

    }

    AST_DUMP(ASTClassStmt) {

    }

}