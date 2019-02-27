//
// Created by 曹顺 on 2019/2/23.
//

#include "class.h"
namespace hypermind {

    HMHash HMMethod::hash() {
        return 0;
    }

    HMMethod::HMMethod(VM *vm, MethodType methodType, HMClosure *fn) : HMObject(vm, ObjectType::Method, vm->mMethodClass) {

    }

}
