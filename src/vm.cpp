//
// Created by 曹顺 on 2019/2/13.
//
#include "vm.h"
#include "obj/class.h"
#include "obj/value.h"
#include "obj/module.h"
#include "obj/map.h"
namespace hypermind {
    bool PrimitiveName(VM *vm, HMInteger argNum, Value *args) {
        if (args[0].objval->classObj == nullptr) {
            args[0] = ((HMClass *)args[0].objval)->name;
        } else {
            args[0] = args[0].objval->classObj->name;
        }
        return true;
    }

    bool PrimitivePrint(VM *vm, HMInteger argNum, Value *args) {
        hm_cout << "---------------" << std::endl;
        args[0].dump(hm_cout);
        hm_cout << std::endl << "---------------" << std::endl;
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

        mMetaClass = NewObject<HMClass>(NewObject<HMString>(_HM_C("Meta")), nullptr, 0);
        mMetaClass->bind(this, Signature(SignatureType::Method, _HM_C("print")), PrimitivePrint);
        mMetaClass->bind(this, Signature(SignatureType::Getter, _HM_C("name")), PrimitiveName);

        mObjectClass = NewObject<HMClass>(NewObject<HMString>(_HM_C("Object")), nullptr, 0);
        mObjectClass->bind(this, Signature(SignatureType::Method, _HM_C("print")), PrimitivePrint);
        mObjectClass->bind(this, Signature(SignatureType::Getter, _HM_C("name")), PrimitiveName);

        mFunctionClass = NewObject<HMClass>(NewObject<HMString>(_HM_C("Function")), nullptr, 0);
        mFunctionClass->bind(this, Signature(SignatureType::Method, _HM_C("call")), HMMethod(MethodType::FunctionCall));
        mFunctionClass->bind(this, Signature(SignatureType::Method, _HM_C("调用")), HMMethod(MethodType::FunctionCall));

        mIntegerClass = NewObject<HMClass>(NewObject<HMString>(_HM_C("Integer")), nullptr, 0);
        mIntegerClass->bind(this, Signature(SignatureType::Method, _HM_C("+")), PrimitiveAdd);
        mIntegerClass->bind(this, Signature(SignatureType::Method, _HM_C("-")), PrimitiveSub);
        mIntegerClass->bind(this, Signature(SignatureType::Method, _HM_C("*")), PrimitiveMul);
        mIntegerClass->bind(this, Signature(SignatureType::Method, _HM_C("/")), PrimitiveDiv);

        module->varNames.Add(&mGCHeap, Signature(_HM_C("Object")));
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
                if (value.objval->classObj == nullptr) {
                    return mMetaClass;
                }
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
