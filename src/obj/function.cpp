//
// Created by 曹顺 on 2019/2/23.
//

#include "function.h"
namespace hypermind {

    HM_OBJ_HASH(Closure) {
        return 0;
    }
    HM_OBJ_FREE(Closure) {
        return false;
    }
    HM_OBJ_DUMP(Closure) {

    }

    HM_OBJ_HASH(Function) {
        return 0;
    }
    HM_OBJ_FREE(Function) {
        return false;
    }
    HM_OBJ_DUMP(Function) {

    }

    HMFunction::HMFunction(VM *vm, HMModule *module) : HMObject(vm,
                                                                                                                 ObjectType::Function,
                                                                                                                 vm->mFunctionClass),
                                                                                                        instructions(Buffer<HMByte>(vm)),
                                                                                                        constants(Buffer<Value>(vm)) {

    }

    FunctionDebug::FunctionDebug(const String &name) : name(name), line(Vector<HMUINT32>()) {

    }
}
