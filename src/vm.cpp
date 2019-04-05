//
// Created by 曹顺 on 2019/2/13.
//

#include "vm.h"
#include "obj/value.h"
#include "obj/module.h"
#include "obj/map.h"
namespace hypermind {
    HMClass* Value::getClass(const VM *vm) const {
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

    bool PrimitiveAdd(VM *vm, Value *args) {
        return true;
    }

    VM::VM() {
//        mAllModule = NewObject<HMMap>();
//        auto *module = NewObject<HMModule>("");
//        mAllModule->set(this, Value(), module);
//        mObjectClass = NewObject<HMClass>(NewObject<HMString>("object"), nullptr, 0);
//        mObjectClass->bind(this, Signature(SignatureType::Method, "="), PrimitiveAdd);

    }
}
