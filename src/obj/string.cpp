//
// Created by 曹顺 on 2019/2/13.
//

#include "string.h"
#include "utils.h"

namespace hypermind {
    HM_OBJ_HASH(String) {
        return hashCode;
    }

    HM_OBJ_FREE(String) {
        charSequence = (HMChar *) vm->MemManger(charSequence, length * sizeof(HMChar), 0);
        return charSequence == nullptr;
    }

    HM_OBJ_DUMP(String) {
        String str(charSequence, length);
        os << " { string(" << sizeof(HMString) << ")   " << str << "   hash : " << hashCode << "  } "
           << std::endl;

    }

}
