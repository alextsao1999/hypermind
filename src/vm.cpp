//
// Created by 曹顺 on 2019/2/13.
//

#include "vm.h"
#include "obj/value.h"
namespace hypermind {
    HMClass* Value::getClass(const hypermind::VM *vm) const {
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
    };


}
