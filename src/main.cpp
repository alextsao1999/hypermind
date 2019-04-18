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
//    auto *source = const_cast<HMChar *>(_HM_C("类 people {变量 age; 创建(yr) {age = yr + 2 };年龄 { return age+1}; };变量 tt = people.创建(11); 变量 ss = tt.年龄;tt.print();"));
//    auto *source = const_cast<HMChar *>(_HM_C("function test(){class people{var age;new(yr){age = yr}age{return age}age=(value){age =value}}return people.new(123)}var value=test();value.age = 444;"));
    auto *source = const_cast<HMChar *>(_HM_C(""
//                                              "class people{"
//                                              "var age;"
//                                              "new(age){"
//                                              "this.age = age;"
//                                              "} ;"
//                                              "age{return age} ; "
//                                              "age=(age){this.age = age;} ;"
//                                              "test(){return 2222;}"
//                                              "};"
//                                              "var value=people.new(234);"
                                              ""
                                              "System.print(value.test() + 1);"
                                              "var global = 1;"
                                              "function test(kk){"
                                              "var a= kk + global;"
                                              "function temp(){"
                                              "return a"
                                              "}"
                                              "return temp"
                                              "}"
                                              "var func = test(123);"
                                              "var pp = func();"
                                              "System.print(pp);"
                                              ));
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
    // 由指令流创建闭包
//    ast->dump(hm_cout);

    auto *closure = vm.NewObject<HMClosure>(compiler.mCurCompileUnit->mFn);
    auto *thread = vm.NewObject<HMThread>(closure);
//

    thread->execute(&vm);
//    compiler.mCurModule->dump(hm_cout);

    vm.mGCHeap.StartGC();
//    vm.DumpAllObjects();

    hm_cout << "----------------------";
//    vm.DumpAllObjects();
    return 0;
}

