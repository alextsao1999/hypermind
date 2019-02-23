//
// Created by 曹顺 on 2019/2/19.
//

#ifndef HYPERMIND_VALUE_H
#define HYPERMIND_VALUE_H

#include "object.h"

namespace hypermind {
    struct HMObject;

    enum class ValueType {
        Undefined,
        Null,
        True,
        False,
        Number,
        Object
    };

    struct Value {
        ValueType type;
        union {
            HMInteger intval;
            HMDouble dbval;
            HMObject *objval;
        };
    };
}


#endif //HYPERMIND_VALUE_H
