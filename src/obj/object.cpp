//
// Created by 曹顺 on 2019/2/19.
//

#include <vm.h>

#include "object.h"
#include "class.h"

namespace hypermind {

    HMHash HMClass::hash() {
        return 0;
    }

    HMObject::HMObject(VM *vm, ObjectType type, HMClass *classObj) {
        next = vm->mAllObjects;
        vm->mAllObjects = this;
    }


    void VM::LinkObject(HMObject *obj) {
        obj->next = mAllObjects;
        mAllObjects = obj;
    }

    void VM::DumpAllObjects() {
        HMObject *nextObj = mAllObjects;
        while (nextObj != nullptr) {
            nextObj->dump();
            nextObj = nextObj->next;
        }
    }


}
