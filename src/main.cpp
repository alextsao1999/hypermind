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
    auto *source = const_cast<HMChar *>(_HM_C("function test(a, b, c) {var ddd = 666+41*3;ddd=100;}"));
    VM vm;
    Lexer lexer(&vm, source);
    Parser parser(_HM_C(""), lexer);
    ASTNodePtr ast = parser.ParseProgram();
    Compiler compiler(&vm);
    CompileUnit cu = compiler.CreateCompileUnit(new FunctionDebug("module"));
    compiler.mCurModule = vm.NewObject<HMModule>(vm.NewObject<HMString>(_HM_C("test"), 4));
    compiler.mCurCompileUnit = &cu;
    ast->compile(&compiler, false);

//    ast->dump(hm_cout);
    vm.DumpAllObjects();

    return 0;
}

