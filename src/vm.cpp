//
// Created by 曹顺 on 2019/2/13.
//
#include "vm.h"
#include "obj/class.h"
#include "obj/value.h"
#include "obj/module.h"
#include "obj/map.h"
namespace hypermind {
    bool PrimitiveAdd(VM *vm, Value *args) {
        return true;
    }

    VM::VM() {
        mAllModule = NewObject<HMMap>();
        auto *module = NewObject<HMModule>(nullptr);
        mAllModule->set(this, Value(), module);

        mObjectClass = NewObject<HMClass>(NewObject<HMString>("Object"), nullptr, 0);
        mObjectClass->bind(this, Signature(SignatureType::Method, "+"), PrimitiveAdd);

        module->varNames.Add(&mGCHeap, Signature("Object"));
        module->varValues.append(&mGCHeap, mObjectClass);

    }

    HMClass *VM::GetValueClass(const Value &value) {
        switch (value.type) {
            case ValueType::Integer:
                return mIntegerClass;
            case ValueType::Float:
                return mFloatClass;
            case ValueType::Undefined:
            case ValueType::Null:
                break;
            case ValueType::Object:
                return (value.objval->classObj);
            case ValueType::True:
            case ValueType::False:
                return mBooleanClass;
            default:
                break;
        }
        return mMetaClass;
    }

}
