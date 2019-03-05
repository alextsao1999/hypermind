//
// Created by 曹顺 on 2019/3/1.
//

#include "ast.h"

namespace hypermind {
    // 语法块Dump
    AST_DUMP(ASTBlock) {
        for (auto &stmt : stmts) {
            stmt->dump(os, ind);
        }
    }

    // 二元表达式Dump
    AST_DUMP(ASTBinary) {
        for (int i = 0; i < ind * 4; ++i) {
            os << _HM_C(" ");
        }
        os << _HM_C(" { (binary)  ");
        mLHS->dump(os, 0);
        op.dump(os);
        mRHS->dump(os, 0);
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

}