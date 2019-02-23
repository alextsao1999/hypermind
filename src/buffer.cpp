//
// Created by 曹顺 on 2019/2/13.
//

#include "buffer.h"

namespace hypermind {
    HMUINT32 CeilToPowerOf2(HMUINT32 v) {
        v += (v == 0);
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        return v;
    }

}