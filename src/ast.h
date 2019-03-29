#include <utility>

#include <utility>

#include <utility>

//
// Created by 曹顺 on 2019/3/1.
//
#ifndef HYPERMIND_AST_H
#define HYPERMIND_AST_H

#include <memory>
#include "hypermind.h"
#include "obj/value.h"
#include "lexer.h"

#define AST_NODE(n,p)  \
    struct n;  \
    using p = std::shared_ptr<n>;  \
    struct n : public ASTNode
#define AST_DECL() void compile(Compiler *compiler, bool isAssign);void dump(Ostream &os);using ASTNode::ASTNode;
#define AST_DUMP(n) void n::dump(Ostream &os)
#define AST_COMPILE(n) void n::compile(Compiler *compiler, bool isAssign)
namespace hypermind {
    class Compiler;
    struct ASTNode {
        HMUINT32 line{0};
        HMUINT32 column{0};
        ASTNode(HMUINT32 line, HMUINT32 column) : line(line), column(column) {};
        ASTNode() = default;
        virtual void dump(Ostream &os){};
        virtual void compile(Compiler *compiler, bool isAssign){};
    };
    using ASTNodePtr = std::shared_ptr<ASTNode>;

    // AST列表
    AST_NODE(ASTList, ASTListPtr) {
        std::vector<ASTNodePtr> elements;
        AST_DECL();
    };

    // 语法块Node
    AST_NODE(ASTBlock, ASTBlockPtr) {
        std::vector<ASTNodePtr> stmts;
        inline void addStmt(ASTNodePtr &&np) {
            // 添加语句 Block/Stmt/Expr
            stmts.push_back(np);
        }
        AST_DECL();
    };

    // 表达式中的变量
    AST_NODE(ASTVariable, ASTVariablePtr){
        Token var;
        ASTListPtr postfix;

        AST_DECL();
    };

    // 值的字面量
    AST_NODE(ASTLiteral, ASTLiteralPtr){
        Value value;
        ASTLiteral(HMUINT32 line, HMUINT32 column, const Value &value) : ASTNode(line, column), value(value) {}
        AST_DECL();
    };

    // 二元表达式
    AST_NODE(ASTBinary, ASTBinaryPtr) {
        TokenType op;
        ASTNodePtr lhs; // 产生式左边
        ASTNodePtr rhs; // 产生式右边
        ASTBinary(HMUINT32 line, HMUINT32 column, TokenType op, ASTNodePtr lhs, ASTNodePtr rhs) : ASTNode(
                line, column), op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

        AST_DECL();
    };

    // If 语句
    AST_NODE(ASTIfStmt, ASTIfStmtPtr) {
        ASTNodePtr condition; // 条件
        // Block or Expr
        ASTNodePtr thenBlock;
        ASTNodePtr elseBlock;
        AST_DECL();
    };

    // while 语句
    AST_NODE(ASTWhileStmt, ASTWhileStmtPtr) {
        ASTNodePtr condition;
        ASTNodePtr block;
        AST_DECL();
    };

    // break 语句
    AST_NODE(ASTBreakStmt, ASTBreakStmtPtr) {
        AST_DECL();
    };

    // continue 语句
    AST_NODE(ASTContinueStmt, ASTContinueStmtPtr) {
        AST_DECL();
    };

    // return 语句
    AST_NODE(ASTReturnStmt, ASTReturnStmtPtr) {
        // 返回值
        ASTNodePtr retvalue;
        AST_DECL();
    };

    // 变量声明语句
    AST_NODE(ASTVarStmt, ASTVarStmtPtr) {
        Token identifier;
        ASTNodePtr value; // 初始值表达式
        ASTVarStmt(HMUINT32 line, HMUINT32 column, const Token &identifier, ASTNodePtr value) :
                ASTNode(line,column), identifier(identifier), value(std::move(value)) {}

        AST_DECL();
    };

    // 参数声明语句
    AST_NODE(ASTParamStmt, ASTParamStmtPtr) {
        Token identifier;
        ASTNodePtr value; // 初始值表达式
        ASTParamStmt(HMUINT32 line, HMUINT32 column, const Token &identifier, ASTNodePtr value) :
                ASTNode(line,column), identifier(identifier), value(std::move(value)) {}

        AST_DECL();
    };

    // AST函数声明
    AST_NODE(ASTFunctionStmt, ASTFunctionStmtPtr) {
        Token name; // 函数名称
        ASTNodePtr params; // 形式参数
        ASTNodePtr body; // 函数体
        AST_DECL();
    };

    // AST类声明
    AST_NODE(ASTClassStmt, ASTClassStmtPtr) {
        Token name; // 类名称
        ASTNodePtr classBody;

        AST_DECL();
    };

    AST_NODE(ASTArgPostfix, ASTArgPostfixPtr) {

        AST_DECL();
    };

    AST_NODE(ASTDotPostfix, ASTDotPostfixPtr) {

        AST_DECL();
    };


}

#endif //HYPERMIND_AST_H
