//
// Created by 曹顺 on 2019/2/13.
//

#include "string.h"
#include "utils.h"
namespace hypermind {

    HMString::HMString(ObjectType type, HMClass *objClass, HMObject *next, const String &str)
            : HMObject(type, objClass, next), str(str) {
        // 计算hash码
        hashCode = hashString(str.c_str(), (HMUINT32) str.length());

    }

    HMUINT32 HMString::hash() {
        return hashCode;
    }

}
