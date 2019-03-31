#include <iostream>
#include "lexer.h"
#include "parser.h"
#include "vm.h"
#include "buffer.h"
#include "obj/value.h"
#include "obj/string.h"
#include "compiler.h"

#include <obj/map.h>
#include "obj/thread.h"

using namespace hypermind;

using namespace std;

int main() {
    auto *source = const_cast<HMChar *>(_HM_C("function test(a, b, c) {var ddd = (1+2)*3;ddd=100;return ddd;} \ntest(1,2,3)"));
//    auto *source = const_cast<HMChar *>(_HM_C("var a = 100; a = a + 20+391;"));
    VM vm;
    Lexer lexer(&vm, source);
    Parser parser(_HM_C(""), lexer);
    ASTNodePtr ast = parser.ParseProgram();
    Compiler compiler(&vm);
    CompileUnit cu = compiler.CreateCompileUnit(new FunctionDebug("module"));
    compiler.mCurModule = vm.NewObject<HMModule>(vm.NewObject<HMString>(_HM_C("test")));
    compiler.mCurCompileUnit = &cu;
    ast->compile(&compiler);
    compiler.mCurCompileUnit->EmitPushNull();
    compiler.mCurCompileUnit->EmitReturn();
    // 由指令流创建闭包
    auto *closure = vm.NewObject<HMClosure>(compiler.mCurCompileUnit->mFn);
    auto *thread = vm.NewObject<HMThread>(closure);
    thread->execute(&vm);
//    vm.DumpAllObjects();
    vm.StartGC();
//    vm.DumpAllObjects();
//    hm_cout << " allocated :  " << vm.GetAllocatedBytes();
    return 0;
}

