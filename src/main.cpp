#include <iostream>
#include <lexer.h>

#include <vm.h>

#include <buffer.h>
#include <tchar.h>

#include "obj/value.h"
#include "obj/string.h"
using namespace hypermind;

using namespace std;

int main() {
    auto *source = const_cast<HMChar *>(R"(int test ='test\n \\ string value' ; )");
    VM vm;
    vm.New<HMString>(&vm, source, 37);
    vm.New<HMString>(&vm, source, 36);
    vm.New<HMString>(&vm, source, 35);
    vm.New<HMString>(&vm, source, 34);
    vm.New<HMString>(&vm, source, 22);
    vm.New<HMString>(&vm, source, 11);
    cout << " allocated bytes: "  << vm.GetAllocatedBytes() << endl;


    cout << "size of Object : " << sizeof(HMObject) << endl;

    vm.DumpAllObjects();


    return 0;
}

