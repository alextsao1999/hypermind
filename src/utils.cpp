//
// Created by 曹顺 on 2019/2/11.
//

#include "utils.h"

namespace hypermind {
    //fnv-1a算法
    HMUINT32 hashString(const char *str, uint32_t length) {
        register HMUINT32 hashCode = 2166136261, idx = 0;
        while (idx < length) {
            hashCode ^= str[idx];
            hashCode *= 16777619;
            idx++;
        }
        return hashCode;
    }

}

