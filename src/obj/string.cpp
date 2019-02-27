//
// Created by 曹顺 on 2019/2/13.
//

#include "string.h"
#include "utils.h"
#include <iostream>
namespace hypermind {
    HMHash HMString::hash() {
        return hashCode;
    }

    HMBool HMString::release(VM *vm) {
        charSequence = (HMChar *) vm->MemManger(charSequence, length * sizeof(HMChar), 0);
        return charSequence == nullptr;

    }

    void HMString::dump() {
        std::string str(charSequence, length * sizeof(HMChar));
        std::cout << " { string value : val ->" << str << "  } ";
    }

}
