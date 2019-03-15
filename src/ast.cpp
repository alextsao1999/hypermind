//
// Created by 曹顺 on 2019/3/1.
//

#include "ast.h"

namespace hypermind {
    // 语法块Dump
    AST_DUMP(ASTBlock) {
        os << _HM_C("  {  ") << std::endl;
        for (auto &stmt : stmts) {
            os << "    " ;
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
        switch (mValue.type) {
            case ValueType::Integer:
                os << _HM_C("[inval : ") << mValue.intval << _HM_C("]");
                break;
            case ValueType::Object:
                mValue.objval->dump(os);
                break;
            case ValueType::Double:
                os << mValue.dbval;
                break;
            case ValueType::Undefined:
                os << _HM_C("[undefined]");
                break;
            case ValueType::Null:
                os << _HM_C("[null]");
                break;
            case ValueType::True:
                os << _HM_C("[true]");
                break;
            case ValueType::False:
                os << _HM_C("[false]");
                break;
        }
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

    }
    AST_DUMP(ASTContinueStmt) {

    }
    AST_DUMP(ASTVarStmt) {

        os << _HM_C(" define var : ") ;
        mIdentifier.dump(os);
        if (mValue != nullptr) {
            os << _HM_C("  value : ");
            mValue->dump(os);
        }
        os << _HM_C("  ") << std::endl;

    }
    AST_DUMP(ASTList) {

    }
    AST_DUMP(ASTFunctionStmt) {

    }
    AST_DUMP(ASTClassStmt) {

    }

}