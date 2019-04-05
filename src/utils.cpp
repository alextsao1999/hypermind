//
// Created by 曹顺 on 2019/2/11.
//

#include "utils.h"

namespace hypermind {
    //fnv-1a算法
    HMHash hashString(const HMChar *str, HMUINT32 length) {
        HMHash hashCode = 2166136261;
        HMUINT32 idx = 0;
        while (idx < length) {
            hashCode ^= str[idx];
            hashCode *= 16777619;
            idx++;
        }
        return hashCode;
    }

    void mixHashString(HMHash &hashCode, const HMChar *str, HMUINT32 length) {
        HMUINT32 idx = 0;
        while (idx < length) {
            hashCode ^= str[idx];
            hashCode *= 16777619;
            idx++;
        }
    }

}

