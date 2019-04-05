//
// Created by 曹顺 on 2019/3/21.
//

#ifndef HYPERMIND_OPCODE_H
#define HYPERMIND_OPCODE_H

#include "hypermind.h"
namespace hypermind {
    enum class Opcode : HMByte {
        LoadConstant,
        LoadLocalVariable,
        StoreLocalVariable,
        LoadModuleVariable,
        StoreModuleVariable,
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
         *  数学运算
         */
         Add,
         Sub,
         Mul,
         Div,

         And,
         Or,


         CreateClosure,
         CloseUpvalue,
         Return,

         // 创建类
         CreateClass,
         Constructor,

         // 绑定实例方法
         BindInstanceMethod,
         // 绑定静态方法
         BindStaticMethod,

         End

    };
}



#endif //HYPERMIND_OPCODE_H
