//
// Created by 曹顺 on 2019/2/19.
//

#include "object.h"
#include "class.h"

namespace hypermind {

    HMHash HMClass::hash() {
        return 0;
    }

    HMObject::HMObject(VM *vm, ObjectType type, HMClass *classObj) {
        next = vm->mAllObjects;
        vm->mAllObjects->next = this;
    }


    void VM::LinkObject(HMObject *obj) {
        obj->next = mAllObjects;
        mAllObjects->next = obj;
    }


}
