//
// Created by ≤‹À≥ on 2019/3/21.
//

#ifndef HYPERMIND_OPCODE_H
#define HYPERMIND_OPCODE_H

#include "hypermind.h"
namespace hypermind {
    enum class Opcode : HMByte {
        LoadConstant,
        LoadLocalVariable,
        StoreLocalVarible,
        LoadModuleVariable,
        StoreModuleVarible,
        LoadUpvalue,
        StoreUpvalue,
        LoadThisField,
        StoreThisField,
        LoadField,
        StoreField,
        Pop,
        PushNull,
        PushTrue,
        PushFalse,
        Call,
        Call0,
        Call1,
        Call2,
        Call3,
        Call4,
        Call5,
        Call6,
        Call7,
        Super,
        Super0,
        Super1,
        Super2,
        Super3,
        Super4,
        Super5,
        Super6,
        Super7,
        Jump,
        Loop,
        JumpIfFalse,
        JumpIfTrue,
        /**
         *   ˝—ß‘ÀÀ„
         */
         Add,
         Sub,
         Mul,
         Div,
        And,
        Or,
        CreateClosure,
        CloseUpval,
        Return,
        CreateClass,
        Constructor,
        InstanceMethod,
        StaticMethod,

    };
}



#endif //HYPERMIND_OPCODE_H
