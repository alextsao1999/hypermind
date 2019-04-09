//
// Created by 曹顺 on 2019/2/13.
//
#include "vm.h"
#include "obj/class.h"
#include "obj/value.h"
#include "obj/module.h"
#include "obj/map.h"
#define METHOD  HMMethod([](VM *vm, HMInteger argNum, Value *args) -> bool

namespace hypermind {
    bool PrimitiveName(VM *vm, HMInteger argNum, Value *args) {
        args[0] = ((HMClass *) (args[0].objval))->name;
        return true;
    }

    bool PrimitiveAdd(VM *vm, HMInteger argNum, Value *args) {
        args[0].intval = args[0].intval + args[1].intval;

        return true;
    }

    bool PrimitiveSub(VM *vm, HMInteger argNum, Value *args) {
        args[0].intval = args[0].intval - args[1].intval;
        return true;
    }

    bool PrimitiveNegative(VM *vm, HMInteger argNum, Value *args) {
        args[0].intval = -args[0].intval;
        return true;
    }

    bool PrimitiveMul(VM *vm, HMInteger argNum, Value *args) {
        args[0].intval = args[0].intval * args[1].intval;
        return true;
    }

    bool PrimitiveDiv(VM *vm, HMInteger argNum, Value *args) {
        args[0].intval = args[0].intval / args[1].intval;
        return true;
    }

    VM::VM() {
        mAllModule = NewObject<HMMap>();
        auto *module = NewObject<HMModule>(nullptr);
        mAllModule->set(this, Value(), module);


        mObjectClass = NewObject<HMClass>(_HM_C("Object"), 6, 0);
        mObjectClass->bind(this, Signature(SignatureType::Getter, _HM_C("name")), PrimitiveName);
        mObjectClass->bind(this, Signature(SignatureType::Getter, _HM_C("class")), METHOD {
            args[0] = args[0].objval->classObj;
            return true;
        }));

        mMetaClass = NewObject<HMClass>(_HM_C("Class"), 5, 0);
        mMetaClass->classObj = mObjectClass;
        mMetaClass->bindSuper(this, mObjectClass);
        mMetaClass->bind(this, Signature(SignatureType::Getter, _HM_C("class")), METHOD {
            args[0] = args[0].objval->classObj;
            return true;
        }));

        auto *objectMeta = NewObject<HMClass>(_HM_C("Object Meta"), 11, 0);
        objectMeta->bind(this, Signature(SignatureType::Getter, _HM_C("name")), PrimitiveName);
        objectMeta->bindSuper(this, mMetaClass);
        mObjectClass->classObj = objectMeta;

        mFunctionClass = NewObject<HMClass>(NewObject<HMString>(_HM_C("Function")), nullptr, 0);
        mFunctionClass->bind(this, Signature(SignatureType::Method, _HM_C("call")), HMMethod(MethodType::FunctionCall));

        mIntegerClass = NewObject<HMClass>(NewObject<HMString>(_HM_C("Integer")), nullptr, 0);
        mIntegerClass->bind(this, Signature(SignatureType::Method, _HM_C("+")), PrimitiveAdd);
        mIntegerClass->bind(this, Signature(SignatureType::Method, _HM_C("-")), PrimitiveSub);
        mIntegerClass->bind(this, Signature(SignatureType::Getter, _HM_C("-")), PrimitiveNegative);
        mIntegerClass->bind(this, Signature(SignatureType::Method, _HM_C("*")), PrimitiveMul);
        mIntegerClass->bind(this, Signature(SignatureType::Method, _HM_C("/")), PrimitiveDiv);

        mBooleanClass = NewObject<HMClass>(NewObject<HMString>(_HM_C("Boolean")), nullptr, 0);
        mBooleanClass->bind(this, Signature(SignatureType::Getter, _HM_C("!")), METHOD {
            args[0].type = args[0].type == ValueType::True ? ValueType::False : ValueType::True;
            return true;
        }));

        auto *mSystemClass = NewObject<HMClass>(NewObject<HMString>(_HM_C("System")), nullptr, 0);
        mSystemClass->classObj->bind(this, Signature(SignatureType::Method, _HM_C("print")), METHOD {
            args[1].dump(hm_cout);
            hm_cout << std::endl;
            return true;
        }));
        module->add(this, _HM_C("Object"), mObjectClass);
        module->add(this, _HM_C("System"), mSystemClass);

        mMapClass = NewObject<HMClass>(NewObject<HMString>(_HM_C("Map")), nullptr, 0);
        mMapClass->bind(this, Signature(SignatureType::SubscriptSetter), METHOD {
            auto *objMap = (HMMap *) args[0].objval;
            objMap->set(vm, args[1], args[2]);
            return true;
        }));
        mMapClass->bind(this, Signature(SignatureType::Subscript), METHOD {
            auto *objMap = (HMMap *) args[0].objval;
            objMap->get(args[1]);
            return true;
        }));
        module->add(this, _HM_C("Map"), mMapClass);

    }

    HMClass *VM::GetValueClass(const Value &value) {
        switch (value.type) {
            case ValueType::Integer:
                return mIntegerClass;
            case ValueType::Float:
                return mFloatClass;
            case ValueType::Undefined:
            case ValueType::Null:
                return mNullClass;
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
