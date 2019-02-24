//
// Created by 曹顺 on 2019/2/13.
//

#ifndef HYPERMIND_STRING_H
#define HYPERMIND_STRING_H

#include <hypermind.h>
#include "object.h"
namespace hypermind {
    struct HMString : public HMObject {
        String str;
        HMUINT32 hashCode;

        HMString(ObjectType type, HMClass *objClass, HMObject *next, const String &str);

        HMUINT32 hash() override;
    };

}

#endif //HYPERMIND_STRING_H
