//
// Created by 曹顺 on 2019/2/19.
//

#ifndef HYPERMIND_VALUE_H
#define HYPERMIND_VALUE_H

#include "object.h"

namespace hypermind {
    enum class ValueType {
        Undefined,
        Null,
        True,
        False,
        Integer,
        Double,
        Object
    };

    //Tagged union
    struct Value {
        ValueType type;
        union {
            HMInteger intval;
            HMDouble dbval;
            HMObject *objval;
        };
        HMHash hash();

    };

    HMHash Value::hash() {
        if (type == ValueType::Object) {
            return objval->hash();
        } else {
            // 先用intval表示hash
            return (HMHash) intval;
        }
    }
}


#endif //HYPERMIND_VALUE_H
