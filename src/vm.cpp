//
// Created by 曹顺 on 2019/2/13.
//
#include "vm.h"
#include "obj/class.h"
#include "obj/value.h"
#include "obj/module.h"
#include "obj/map.h"
#define METHOD  HMMethod([](VM *vm, HMInteger argNum, Value *args) -> bool
#define RETURN(value) args[0] = value;return true;
#define RETURN_INT(value) args[0].intval = value;return true;
#define RETURN_TYPE(t) args[0].type = t;return true;
namespace hypermind {
    bool PrimitiveName(VM *vm, HMInteger argNum, Value *args) {
        RETURN(((HMClass *) (args[0].objval))->name);
    }

    bool PrimitiveAdd(VM *vm, HMInteger argNum, Value *args) {
        RETURN_INT(args[0].intval + args[1].intval);
    }

    bool PrimitiveSub(VM *vm, HMInteger argNum, Value *args) {
        RETURN_INT(args[0].intval - args[1].intval);
    }

    bool PrimitiveNegative(VM *vm, HMInteger argNum, Value *args) {
        RETURN_INT(-args[0].intval);
    }

    bool PrimitiveMul(VM *vm, HMInteger argNum, Value *args) {
        RETURN_INT(args[0].intval * args[1].intval);
    }

    bool PrimitiveDiv(VM *vm, HMInteger argNum, Value *args) {
        RETURN_INT(args[0].intval / args[1].intval);
    }

    VM::VM() : mGCHeap(this) {
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
        mIntegerClass->bind(this, Signature(SignatureType::Method, _HM_C("==")), METHOD {
            RETURN_TYPE(args[0].intval == args[1].intval ? ValueType::True : ValueType::False);
        }));
        mIntegerClass->bind(this, Signature(SignatureType::Method, _HM_C(">=")), METHOD {
            RETURN_TYPE(args[0].intval >= args[1].intval ? ValueType::True : ValueType::False);
        }));
        mIntegerClass->bind(this, Signature(SignatureType::Method, _HM_C("<=")), METHOD {
            RETURN_TYPE(args[0].intval <= args[1].intval ? ValueType::True : ValueType::False);
        }));
        mIntegerClass->bind(this, Signature(SignatureType::Method, _HM_C(">")), METHOD {
            RETURN_TYPE(args[0].intval > args[1].intval ? ValueType::True : ValueType::False);
        }));
        mIntegerClass->bind(this, Signature(SignatureType::Method, _HM_C("<")), METHOD {
            RETURN_TYPE(args[0].intval < args[1].intval ? ValueType::True : ValueType::False);
        }));
        mIntegerClass->bind(this, Signature(SignatureType::Method, _HM_C("!=")), METHOD {
            RETURN_TYPE(args[0].intval != args[1].intval ? ValueType::True : ValueType::False);
        }));
        mIntegerClass->bind(this, Signature(SignatureType::Method, _HM_C("+")), PrimitiveAdd);
        mIntegerClass->bind(this, Signature(SignatureType::Method, _HM_C("-")), PrimitiveSub);
        mIntegerClass->bind(this, Signature(SignatureType::Getter, _HM_C("-")), PrimitiveNegative);
        mIntegerClass->bind(this, Signature(SignatureType::Method, _HM_C("*")), PrimitiveMul);
        mIntegerClass->bind(this, Signature(SignatureType::Method, _HM_C("/")), PrimitiveDiv);

        mStringClass = NewObject<HMClass>(NewObject<HMString>(_HM_C("String")), nullptr, 0);

        mBooleanClass = NewObject<HMClass>(NewObject<HMString>(_HM_C("Boolean")), nullptr, 0);
        mBooleanClass->bind(this, Signature(SignatureType::Getter, _HM_C("!")), METHOD {
                           RETURN_TYPE(args[0].type == ValueType::True ? ValueType::False : ValueType::True);
       }));

        auto *mSystemClass = NewObject<HMClass>(NewObject<HMString>(_HM_C("System")), nullptr, 0);
        mSystemClass->classObj->bind(this, Signature(SignatureType::Method, _HM_C("print")), METHOD {
            args[1].dump(hm_cout);
            hm_cout << std::endl;
            RETURN_TYPE(ValueType::True);
        }));

        mMapClass = NewObject<HMClass>(NewObject<HMString>(_HM_C("Map")), nullptr, 0);
        mMapClass->classObj->bind(this, Signature(SignatureType::Constructor), METHOD {
                           RETURN(Value(vm->NewObject<HMMap>()));
        }));
        mMapClass->bind(this, Signature(SignatureType::SubscriptSetter), METHOD {
            auto *objMap = (HMMap *) args[0].objval;
            objMap->set(vm, args[1], args[2]);
            RETURN_TYPE(ValueType::True);
        }));
        mMapClass->bind(this, Signature(SignatureType::Subscript), METHOD {
            auto *objMap = (HMMap *) args[0].objval;
            RETURN(objMap->get(args[1]));
        }));

        module->add(this, _HM_C("Object"), mObjectClass);
        module->add(this, _HM_C("System"), mSystemClass);
        module->add(this, _HM_C("Map"), mMapClass);
        module->add(this, _HM_C("String"), mStringClass);
        module->add(this, _HM_C("Function"), mFunctionClass);
        module->add(this, _HM_C("Integer"), mIntegerClass);
        module->add(this, _HM_C("Boolean"), mBooleanClass);

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
