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
    auto *source = const_cast<HMChar *>(_HM_C("function test(a, b, c) {aa = 444}"));
    VM vm;
    Lexer lexer(&vm, source);
    Parser parser(_HM_C(""), lexer);
    ASTNodePtr ast = parser.ParseProgram();
    ast->dump(hm_cout);

    //    vm.DumpAllObjects();


    return 0;
}

