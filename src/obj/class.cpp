//
// Created by 曹顺 on 2019/2/23.
//

#include "class.h"
namespace hypermind {

    HM_OBJ_HASH(Method) {
        return 0;
    }
    HM_OBJ_FREE(Method) {
        return false;
    }
    HM_OBJ_DUMP(Method) {

    }

    HM_OBJ_HASH(Class) {
        return 0;
    }
    HM_OBJ_FREE(Class) {
        return false;
    }
    HM_OBJ_DUMP(Class) {

    }

    HM_OBJ_HASH(Instance) {

        return 0;
    }
    HM_OBJ_FREE(Instance) {
        return false;
    }
    HM_OBJ_DUMP(Instance) {

    }

    HMMethod::HMMethod(VM *vm, MethodType methodType, HMClosure *fn) : HMObject(vm, ObjectType::Method, vm->mMethodClass) {

    }

}
