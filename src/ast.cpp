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
            os << " ";
        }
        os << "binary : " ;
        mLHS->dump(os, 0);
        op.dump(os);
        mRHS->dump(os, 0);
        os << std::endl;
    }

    // 字面量Dump
    AST_DUMP(ASTLiteral) {
        switch (mValue.type) {
            case ValueType::Integer:
                os << "[inval : " << mValue.intval << "]";
                break;
            case ValueType::Object:
                mValue.objval->dump(os);
                break;
            case ValueType::Double:
                os << mValue.dbval;
                break;
            case ValueType::Undefined:
                os << "[undefined]";
                break;
            case ValueType::Null:
                os << "[null]";
                break;
            case ValueType::True:
                os << "[true]";
                break;
            case ValueType::False:
                os << "[false]";
                break;
        }
    }

    AST_DUMP(ASTVariable) {
        // 字面量Dump
        mVar.dump(os);

    }

}