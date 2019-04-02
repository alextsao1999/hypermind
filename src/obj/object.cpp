//
// Created by 曹顺 on 2019/2/19.
//
#include "object.h"
#include <vm.h>

namespace hypermind {

//    void VM::LinkObject(HMObject *obj) {
//        obj->next = mAllObjects;
//        mAllObjects = obj;
//    }
//
//    void VM::DumpAllObjects() {
//        HMObject *nextObj = mAllObjects;
//        while (nextObj != nullptr) {
//            nextObj->dump(hm_cout);
//            hm_cout << std::endl;
//            nextObj = nextObj->next;
//        }
//    }

    HMObject::HMObject(VM *vm, ObjectType type, HMClass *classObj) : type(type), classObj(classObj) {
        next = vm->mGCHeap.mAllObjects;
        vm->mGCHeap.mAllObjects = this;
    }
}
