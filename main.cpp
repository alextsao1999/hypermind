#include <iostream>
#include <lexer.h>

using namespace hypermind;

int main() {
    hypermind::Lexer lexer("int a = 1 + 2;1234;11;");

    hypermind::Token token;
    while ((token = lexer.read()).mType != TK_EOF){
        hypermind::dumpToken(token);
    }

    return 0;
}

