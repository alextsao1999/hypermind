#include <iostream>
#include <lexer.h>

#include <vm.h>

#include <buffer.h>

using namespace hypermind;

using namespace std;
int main() {
    auto *source = const_cast<HMChar *>("int test ='test\\n \\\\ string value' ; ");
    VM vm;

    Lexer lexer(&vm, source);

    Token token;
    while ((token = lexer.read()).mType != TK_EOF) {
        dumpToken(token);

    }


    return 0;
}

