//
// Created by ≤‹À≥ on 2019/3/5.
//

#include "thread.h"

namespace hypermind {
    HM_OBJ_DUMP(Thread) {

    }
    HM_OBJ_HASH(Thread) {
        return 0;
    }
    HM_OBJ_FREE(Thread) {
        return false;
    }
}