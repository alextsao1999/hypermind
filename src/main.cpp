#include <iostream>
#include <lexer.h>
#include <parser.h>
#include <vm.h>

#include <buffer.h>
#include <tchar.h>

#include "obj/value.h"
#include "obj/string.h"
using namespace hypermind;

using namespace std;

int main() {
    auto *source = const_cast<HMChar *>("1+2*3");
    VM vm;
    Lexer lexer(&vm, source);
    Parser parser("", lexer);
    ASTExprPtr ast = parser.parseExpression();
    ast->dump(cout, 0);

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

