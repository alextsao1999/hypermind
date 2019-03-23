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
        HMHash hashCode;
        HMUINT32 length;
        HMChar *charSequence;
        HM_OBJ_CONSTRUTOR(String, const HMChar *str, HMUINT32 len), length(len) {
            // 计算文本hash
            charSequence = vm->Allocate<HMChar>(length);
            if (charSequence != nullptr) {
                hashCode = hashString(charSequence, length);
                hm_memcpy(charSequence, str, length);
            } else {
                MEMRY_ERROR("文本对象申请内存失败");
            }
        }
        HM_OBJ_DECL();

    };

}

#endif //HYPERMIND_STRING_H
