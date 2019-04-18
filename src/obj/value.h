//
// Created by 曹顺 on 2019/2/19.
//

#ifndef HYPERMIND_VALUE_H
#define HYPERMIND_VALUE_H

#include "object.h"

namespace hypermind {
    class VM;
    enum class ValueType {
        Undefined,
        Null,
        True,
        False,
        Integer,
        Float,
        Object
    };

    //Tagged union
    struct Value {
        ValueType type;
        union {
            HMInteger intval{0};
            HMFloat floatval;
            HMObject *objval;
        };

        Value() : type(ValueType::Null) {};
        Value(ValueType type) : type(type) {};
        Value(HMObject *obj) : type(ValueType::Object), objval(obj) {};
        Value(HMInteger val) : type(ValueType::Integer), intval(val) {};
        Value(HMFloat val) : type(ValueType::Float), floatval(val) {};
        Value(HMBool val) : type(val ? ValueType::True : ValueType::False) {};

        inline HMHash hash() const {
            if (type == ValueType::Object) {
                return objval->hash();
            } else {
                // 先用intval表示hash
                return (HMHash) intval;
            }
        };

        void dump(Ostream &os) const {
            switch (type) {
                case ValueType::Integer:
                    os << _HM_C("[integer : ") << intval << _HM_C("]");
                    break;
                case ValueType::Object:
                    os << _HM_C("[pointer : ") << static_cast<const void *>(objval) << _HM_C("]");
//                    objval->dump(os);
                    break;
                case ValueType::Float:
                    os << _HM_C("[float : ") << floatval << _HM_C("]");
                    break;
                case ValueType::Undefined:
                    os << _HM_C("[undefined]");
                    break;
                case ValueType::Null:
                    os << _HM_C("[null]");
                    break;
                case ValueType::True:
                    os << _HM_C("[true]");
                    break;
                case ValueType::False:
                    os << _HM_C("[false]");
                    break;
            }

        }

        inline HMBool equals(Value &value) {
            if (type != value.type)
                return false;
            switch (type) {
                case ValueType::Undefined:
                case ValueType::Null:
                case ValueType::True:
                case ValueType::False:
                    return true;
                case ValueType::Integer:
                    return intval == value.intval;
                case ValueType::Float:
                    return floatval == value.floatval;
                case ValueType::Object:
                    if (objval == value.objval)
                        return true;
                    return objval->equals(value.objval);
                default:
                    return false;
            }

        }

    };
}


#endif //HYPERMIND_VALUE_H
