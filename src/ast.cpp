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

    ASTBinary::ASTBinary(const Token &op, const ASTExprPtr &mLHS, const ASTExprPtr &mRHS) : op(op), mLHS(mLHS),
                                                                                            mRHS(mRHS) {}

    // 字面量Dump
    AST_DUMP(ASTLiteral) {
        os << mValue.dbval << std::endl;

    }
    AST_DUMP(ASTVariable) {
        // 字面量Dump
        mVar.dump(os);

    }

}