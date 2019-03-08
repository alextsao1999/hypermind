#include <iostream>
#include <lexer.h>
#include <parser.h>
#include <vm.h>

#include <buffer.h>
#include <tchar.h>

#include "obj/value.h"
#include "obj/string.h"
#include <windows.h>
using namespace hypermind;

using namespace std;

int main() {
    auto *source = const_cast<HMChar *>(_HM_C("asdf=asdf+1234"));
    VM vm;
    Lexer lexer(&vm, source);

    Parser parser(_HM_C(""), lexer);
    ASTExprPtr ast = parser.ParseExpression();
    ast->dump(HMCout, 0);

    //    vm.DumpAllObjects();


    return 0;
}

