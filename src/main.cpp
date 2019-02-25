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


    exit(0);



    auto *source = const_cast<HMChar *>(R"(int test ='test\n \\ string value' ; )");
    VM vm;

    Buffer<HMInteger > buf(&vm);
    buf.Append(111);
    buf.Append(2222);
    buf.Append(333);

    buf.Insert(5, 90);

    //buf.Remove(3);


    buf.Dump();
    std::cout << "长度 : " << buf.GetCount();



    return 0;

    //Lexer lexer(&vm, source);
//    Token token;
//    while ((token = lexer.Read()).mType != TokenType::End) {
//        DumpToken(token);
//
//    }
//
//
//    return 0;
}

