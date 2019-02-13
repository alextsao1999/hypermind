#include <iostream>
#include <lexer.h>

#include <vm.h>

#include <buffer.h>

using namespace hypermind;

using namespace std;
int main() {
    VM vm;
    Buffer<int> buf(&vm);

    for (int i = 0; i < 101; ++i) {
        buf.add(1234);
    }
    buf[120] = 777;
    buf[0] = 666;

    std::cout << buf[119] << "   数量 : " << buf.getCount() << std::endl;

    return 0;
}

