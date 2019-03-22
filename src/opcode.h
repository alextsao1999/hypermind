//
// Created by ≤‹À≥ on 2019/3/21.
//

#ifndef HYPERMIND_OPCODE_H
#define HYPERMIND_OPCODE_H

#include "hypermind.h"
namespace hypermind {
    enum class Opcode : HMByte {
        LoadConstant,
        PushNull,
        PushTrue,
        PushFalse,
        Call0,
        Call1,
        Call2,
        Call3,
        Call4,
        Call5,
        Call6,
        Call7,

    };
}



#endif //HYPERMIND_OPCODE_H
