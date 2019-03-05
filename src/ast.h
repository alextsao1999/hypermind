//
// Created by 曹顺 on 2019/3/1.
//
#ifndef HYPERMIND_AST_H
#define HYPERMIND_AST_H

#include "hypermind.h"
#include <memory>
#include "obj/value.h"
#include "lexer.h"

#define AST_NODE(n,p)  \
    struct n;  \
    using p = std::shared_ptr<n>;  \
    struct n : public ASTNode
#define AST_STMT(n, p) \
    struct n;  \
    using p = std::shared_ptr<n>;  \
    struct n : public ASTStmt
#define AST_EXPR(n, p) \
    struct n;  \
    using p = std::shared_ptr<n>;  \
    struct n : public ASTExpr
#define AST_DECL() void codegen(){};void dump(Ostream &os, int ind);
#define AST_DUMP(n) void n::dump(Ostream &os, int ind)
namespace hypermind {
    struct ASTNode {
        virtual void dump(Ostream &os, int ind){};
        virtual void codegen(){};
    };
    using ASTNodePtr = std::shared_ptr<ASTNode>;


    // 表达式Node
    AST_NODE(ASTExpr, ASTExprPtr) {

    };

    // 语句Node
    AST_NODE(ASTStmt, ASTStmtPtr) {

    };

    // 语法块Node
    AST_NODE(ASTBlock, ASTBlockPtr) {
        std::vector<ASTStmtPtr> stmts;
        inline void addStmt(ASTStmtPtr &&np) {
            stmts.push_back(np);
        }
        AST_DECL();
    };

    AST_EXPR(ASTVariable, ASTVariablePtr){
        Token mVar;
        AST_DECL();
    };

    // 值的字面量
    AST_EXPR(ASTLiteral, ASTLiteralPtr){
        Value mValue{};
        AST_DECL();
    };

    // 二元表达式
    AST_EXPR(ASTBinary, ASTBinaryPtr) {
        Token op;
        ASTExprPtr mLHS; // 产生式左边
        ASTExprPtr mRHS; // 产生式右边
        //ASTBinary(const Token &op, const ASTExprPtr &mLHS, const ASTExprPtr &mRHS);
        AST_DECL();
    };

    // If 语句
    AST_STMT(ASTIfStmt, ASTIfStmtPtr) {
        ASTExprPtr mCondition; // 条件
        ASTBlockPtr mThen;
        ASTBlockPtr mElse;
        AST_DECL();
    };


}

#endif //HYPERMIND_AST_H
