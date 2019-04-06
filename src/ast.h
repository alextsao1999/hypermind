//
// Created by 曹顺 on 2019/3/1.
//
#ifndef HYPERMIND_AST_H
#define HYPERMIND_AST_H

#include <memory>
#include "hypermind.h"
#include "obj/value.h"
#include "lexer.h"
#include "symbol.h"

#define AST_NODE(n,p)  \
    struct n;  \
    using p = std::shared_ptr<n>;  \
    struct n : public ASTNode
#define AST_DECL() void compile(Compiler *compiler, CompileFlag flag);void dump(Ostream &os);using ASTNode::ASTNode;
#define AST_DUMP(n) void n::dump(Ostream &os)
#define AST_COMPILE(n) void n::compile(Compiler *compiler, CompileFlag flag)
namespace hypermind {
    enum class CompileFlag;
    class Compiler;
    struct ASTNode {
        HMUINT32 line{0};
        HMUINT32 column{0};
        ASTNode(HMUINT32 line, HMUINT32 column) : line(line), column(column) {};
        ASTNode() = default;
        virtual void dump(Ostream &os){};
        // 虚函数不允许默认参数 不过这样能运行 我太懒了 直接就这样吧 (●'◡'●)
        virtual void compile(Compiler *compiler, CompileFlag flag = (CompileFlag) 0) {};
    };
    using ASTNodePtr = std::shared_ptr<ASTNode>;

    // AST列表
    AST_NODE(ASTList, ASTListPtr) {
        std::vector<ASTNodePtr> elements;
        inline void addItem(ASTNodePtr &&np) {
            // 添加语句 Block/Stmt/Expr
            elements.push_back(np);
        }
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
        ASTVariable(HMUINT32 line, HMUINT32 column, const Token &var) : ASTNode(line, column), var(var) {}
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
        Token op;
        ASTNodePtr lhs; // 产生式左边
        ASTNodePtr rhs; // 产生式右边
        ASTBinary(HMUINT32 line, HMUINT32 column, Token op, ASTNodePtr lhs, ASTNodePtr rhs) : ASTNode(
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
        ASTListPtr params; // 形式参数
        ASTNodePtr body; // 函数体
        AST_DECL();
    };

    AST_NODE(ASTMethodStmt, ASTMethodStmtPtr) {
        Signature name;
        ASTNodePtr params; // 形式参数
        ASTNodePtr body; // 函数体
        AST_DECL();
    };

    // AST类声明
    AST_NODE(ASTClassStmt, ASTClassStmtPtr) {
        Token name; // 类名称
        Token super;
        std::vector<ASTVarStmtPtr> fields; // 域
        std::vector<ASTNodePtr> statics; // 静态的域或方法
        std::vector<ASTMethodStmtPtr> methods; // 方法
        AST_DECL();
    };

    AST_NODE(ASTNegativeExpr, ASTNegativeExprPtr) {
        ASTNodePtr expr;
        ASTNegativeExpr(HMUINT32 line, HMUINT32 column, ASTNodePtr expr) : ASTNode(line, column), expr(std::move(expr)) {}
        AST_DECL();
    };

    AST_NODE(ASTNotExpr, ASTNotExprPtr) {
        ASTNodePtr expr;
        ASTNotExpr(HMUINT32 line, HMUINT32 column, ASTNodePtr expr) : ASTNode(line, column), expr(std::move(expr)) {}
        AST_DECL();
    };

    AST_NODE(ASTArgPostfix, ASTArgPostfixPtr) {
        ASTNodePtr expr;
        ASTListPtr args;
        ASTArgPostfix(HMUINT32 line, HMUINT32 column, ASTNodePtr expr, ASTListPtr args) : ASTNode(line, column),
                                                                                                        expr(std::move(expr)),
                                                                                                        args(std::move(args)) {

        }
        AST_DECL();
    };

    AST_NODE(ASTDotPostfix, ASTDotPostfixPtr) {
        ASTNodePtr expr;
        Token name;
        ASTDotPostfix(HMUINT32 line, HMUINT32 column, ASTNodePtr expr, const Token &name) : ASTNode(line, column),
                                                                                                   expr(std::move(expr)),
                                                                                                   name(name) {}
        AST_DECL();
    };


}

#endif //HYPERMIND_AST_H
