include_directories(.)

set(FILES main.cpp buffer.cpp lexer.cpp parser.cpp utils.cpp utils.h  vm.cpp obj/string.cpp ast/ASTBase.cpp ast/ASTBase.h ast/ASTNode.cpp ast/ASTNode.h ast/ASTExpr.cpp ast/ASTExpr.h ast/ASTBinary.cpp ast/ASTBinary.h)

add_executable(HyperMind ${FILES})
