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
    auto *source = const_cast<HMChar *>(_HM_C(
            "类 people {变量 age; 创建(yr) {age = yr + 2 };年龄 { return age+1}; };变量 tt = people.创建(11); 变量 ss = tt.年龄;"
            ));
//    auto *source = const_cast<HMChar *>(_HM_C("var a = 100; a = a + 20+391;"));
    VM vm;
    Lexer lexer(&vm, source);
    Parser parser(_HM_C(""), lexer);
    ASTNodePtr ast = parser.ParseProgram();
//    ast->dump(hm_cout);

    Compiler compiler(&vm);
    compiler.mCurModule = vm.NewObject<HMModule>(vm.NewObject<HMString>(_HM_C("test")));
    CompileUnit cu = compiler.CreateCompileUnit(new FunctionDebug(_HM_C("module")));
    compiler.mCurCompileUnit = &cu;

    ast->compile(&compiler);

    compiler.mCurCompileUnit->EmitPushNull();
    compiler.mCurCompileUnit->EmitReturn();
    // 由指令流创建闭包
    auto *closure = vm.NewObject<HMClosure>(compiler.mCurCompileUnit->mFn);
    auto *thread = vm.NewObject<HMThread>(closure);
//    vm.DumpAllObjects();
    thread->execute(&vm);
    compiler.mCurModule->dump(hm_cout);
    vm.mGCHeap.StartGC();
    system("pause");

//    vm.DumpAllObjects();
    return 0;
}

