//
// Created by 曹顺 on 2019/2/13.
//

#include "buffer.h"

namespace hypermind {
    uint32_t ceilToPowerOf2(uint32_t v) {
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