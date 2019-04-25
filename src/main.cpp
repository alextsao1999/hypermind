#include <iostream>
#include "lexer.h"
#include "parser.h"
#include "vm.h"
#include "obj/value.h"
#include "obj/string.h"
#include "compiler.h"
#include "obj/thread.h"

using namespace hypermind;

using namespace std;

int main() {
    auto *source = R"(

class Value  {
    public var value;
    new(val) {
        value = val;
    }
}
var test = Value.new(1234);
test.value = 5555;
System.print(test.value);

)";
    VM vm;
    Lexer lexer(&vm, source);
    Parser parser(_HM_C(""), lexer);
    ASTNodePtr ast = parser.ParseProgram();
    Compiler compiler(&vm);
    compiler.mCurModule = vm.NewObject<HMModule>(vm.NewObject<HMString>(_HM_C("test")));
    CompileUnit cu = compiler.CreateCompileUnit(new FunctionDebug(_HM_C("module")));
    compiler.mCurCompileUnit = &cu;
    ast->compile(&compiler);
    compiler.mCurCompileUnit->EmitPushNull();
    compiler.mCurCompileUnit->EmitReturn();
    auto *thread = vm.NewThread(cu.mFn);
//    vm.DumpAllObjects();

    thread->execute(&vm);
    vm.mGCHeap.StartGC();

    return 0;
}

