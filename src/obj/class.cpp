//
// Created by 曹顺 on 2019/2/23.
//

#include "class.h"
namespace hypermind {

    HMHash HMMethod::hash() {
        return 0;
    }

    HMHash HMClass::hash() {
        return 0;
    }

    HMInstance::HMInstance(ObjectType type, HMClass *objClass, HMObject *next) : HMObject(type, objClass, next) {}
}
