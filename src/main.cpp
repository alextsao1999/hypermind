#include <iostream>
#include "lexer.h"
#include "parser.h"
#include "vm.h"
#include "buffer.h"
#include "obj/value.h"
#include "obj/string.h"

using namespace hypermind;

using namespace std;

int main() {
    auto *source = const_cast<HMChar *>(_HM_C("if (a=1234) {b = 234;if(d=123) {e = 444;k=6666} else pp = 1234;c=5312}"));
    VM vm;
    Lexer lexer(&vm, source);
    Parser parser(_HM_C(""), lexer);
    ASTNodePtr ast = parser.ParseProgram();
    ast->dump(HMCout);

    //    vm.DumpAllObjects();


    return 0;
}

