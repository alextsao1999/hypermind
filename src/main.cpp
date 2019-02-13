#include <iostream>
#include <lexer.h>

#include <vm.h>

#include <buffer.h>

using namespace hypermind;

int main() {
    VM vm;
    Buffer<int> buf(&vm);

    buf.set(1234);
    std::cout << *(buf.get()) << std::endl;

    buf.clear();


    return 0;
}

