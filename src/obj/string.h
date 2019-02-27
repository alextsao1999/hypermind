//
// Created by 曹顺 on 2019/2/13.
//

#ifndef HYPERMIND_STRING_H
#define HYPERMIND_STRING_H

#include <hypermind.h>
#include "vm.h"
#include "object.h"
#include "utils.h"

namespace hypermind {
    struct HMString : public HMObject {
        HMHash hashCode{};
        HMUINT32 length{};
        HMChar *charSequence{nullptr};

        explicit HMString(VM *vm, const HMChar *str, HMUINT32 length) : HMObject(vm, ObjectType::String,
                                                                                 vm->mStringClass){
            // 计算文本hash
            hashCode = hashString(charSequence, length);
            HMUINT32 size = length * sizeof(HMChar);
            charSequence = (HMChar *) vm->Allocate(size);
            memcpy(charSequence, str, size);
        };

        HMHash hash() override;

        HMBool release(VM *vm) override;

        void dump() override;
    };

}

#endif //HYPERMIND_STRING_H
