#include <iostream>
#include "lexer.h"
#include "parser.h"
#include "vm.h"
#include "buffer.h"
#include "obj/value.h"
#include "obj/string.h"
#include "compiler.h"

using namespace hypermind;

using namespace std;

int main() {
    auto *source = const_cast<HMChar *>(_HM_C("function test(a, b, c) {aa = 444}"));
    VM vm;
    Lexer lexer(&vm, source);
    Parser parser(_HM_C(""), lexer);
    ASTNodePtr ast = parser.ParseProgram();
    Compiler compiler(&vm);
    CompileUnit cu(&vm);

    auto *str = vm.NewObject<HMString>(_HM_C("test\0"), 5);
    compiler.mCurModule = vm.NewObject<HMModule>(str);
    compiler.mCurCompileUnit = &cu;
//    ast->dump(hm_cout);

    ast->compile(&compiler, false);

//    ast->dump(hm_cout);
        vm.DumpAllObjects();


    return 0;
}

