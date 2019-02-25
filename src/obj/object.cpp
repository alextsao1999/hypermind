//
// Created by 曹顺 on 2019/2/19.
//

#include "object.h"
namespace hypermind {

    bool HMObject::release() {
        return false;
    }

    HMObject::HMObject(VM *vm, ObjectType type, HMClass *classObj) : type(type), classObj(classObj) {
        vm->LinkObject(this);
    }

}
