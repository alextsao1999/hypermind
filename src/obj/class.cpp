//
// Created by 曹顺 on 2019/2/23.
//

#include "class.h"
namespace hypermind {
    HM_OBJ_HASH(Class) {
        return 0;
    }
    HM_OBJ_FREE(Class) {
        return false;
    }
    HM_OBJ_DUMP(Class) {
        os << _HM_C(" { HMClass(") << sizeof(HMClass) << _HM_C(") ") << static_cast<const void *>(this)
           << _HM_C("  name : ");
        if (name != nullptr) {
            name->dump(os);
        }
        os << std::endl;
        if (superClass != nullptr) {
            os << _HM_C("super [pointer : ") << static_cast<const void *>(superClass) << _HM_C("] ");
        }
        os << _HM_C("  field number : ") << fieldNubmer;
        os << _HM_C(" } ");
        os << std::endl;

    }

    HM_OBJ_HASH(Instance) {

        return 0;
    }
    HM_OBJ_FREE(Instance) {
        return false;
    }
    HM_OBJ_DUMP(Instance) {
        os << _HM_C(" { HMInstance(") << sizeof(HMInstance) << _HM_C(") ") << static_cast<const void *>(this)
           << _HM_C("  Instance class : ") << static_cast<const void *>(classObj) << "   ";
        for (int i = 0; i < classObj->fieldNubmer; ++i) {
            os << "    " << i << " : ";
            fields[i].dump(os);
            os << " | ";
        }
        os << _HM_C(" } ");
        os << std::endl;

    }

}
