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
        Float,
        Object
    };

    //Tagged union
    struct Value {
        ValueType type;
        union {
            HMInteger intval;
            HMFloat floatval;
            HMObject *objval;
        };
        HMHash hash() const {
            if (type == ValueType::Object) {
                return objval->hash();
            } else {
                // 先用intval表示hash
                return (HMHash) intval;
            }
        };

        void dump(Ostream &os) {
            switch (type) {
                case ValueType::Integer:
                    os << _HM_C("[intval : ") << intval << _HM_C("]");
                    break;
                case ValueType::Object:
                    objval->dump(os);
                    break;
                case ValueType::Float:
                    os << _HM_C("[floatval : ") << floatval << _HM_C("]");
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

        HMClass *GetClass(const VM *vm) const {
            switch (type) {
                case ValueType::Integer:
                    return vm->mIntegerClass;
                case ValueType::Float:
                    return vm->mFloatClass;
                case ValueType::Undefined:
                case ValueType::Null:
                    break;
                case ValueType::Object:
                    return objval->classObj;
                case ValueType::True:
                case ValueType::False:
                    return vm->mBooleanClass;
                default:
                    break;
            }
            return vm->mMetaClass;

        }
    };
}


#endif //HYPERMIND_VALUE_H
