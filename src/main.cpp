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
    auto a = L"啊";


    auto *source = const_cast<HMChar *>(_HM_C("a=111+222+333;\n测试=1234"));
    VM vm;
    Lexer lexer(&vm, source);

    lexer.Read().dump(wcout);
    lexer.Read().dump(wcout);
    lexer.Read().dump(wcout);
    lexer.Read().dump(wcout);
    lexer.Read().dump(wcout);
    lexer.Read().dump(wcout);
    lexer.Read().dump(wcout);
    lexer.Read().dump(wcout);
    lexer.Read().dump(wcout);
    lexer.Read().dump(wcout);
    lexer.Read().dump(wcout);
    lexer.Read().dump(wcout);
    lexer.Read().dump(wcout);
    lexer.Read().dump(wcout);
    lexer.Read().dump(wcout);
//    lexer.Read().dump(wcout);
//    lexer.Read().dump(wcout);


    //Parser parser("", lexer);
    //ASTExprPtr ast = parser.ParseExpression();
    //ast->dump(cout, 0);

//    vm.New<HMString>(&vm, source, 37);
//    vm.New<HMString>(&vm, source, 36);
//    vm.New<HMString>(&vm, source, 35);
//    vm.New<HMString>(&vm, source, 34);
//    vm.New<HMString>(&vm, source, 22);
//    vm.New<HMString>(&vm, source, 11);
//    cout << " allocated bytes: "  << vm.GetAllocatedBytes() << endl;
//
//
//    cout << "size of Object : " << sizeof(HMObject) << endl;
//
//    vm.DumpAllObjects();


    return 0;
}

