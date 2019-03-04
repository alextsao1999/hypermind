//
// Created by 曹顺 on 2019/2/13.
//

#ifndef HYPERMIND_STRING_H
#define HYPERMIND_STRING_H

#include <hypermind.h>
#include "vm.h"
#include "object.h"
#include "utils.h"
#include <memory>
namespace hypermind {
    HM_OBJECT(String) {
        HMHash hashCode{};
        HMUINT32 length{};
        HMChar *charSequence{nullptr};

        explicit HMString(VM *vm, const HMChar *str, HMUINT32 length) : HMObject(vm, ObjectType::String,
                                                                                 vm->mStringClass), length(length) {
            HMUINT32 size = length * sizeof(HMChar);
            // 计算文本hash
            charSequence = (HMChar *) vm->Allocate(size);
            hashCode = hashString(charSequence, length);
            memcpy(charSequence, str, size);
        };
        HM_OBJ_DECL();
    };

}

#endif //HYPERMIND_STRING_H
