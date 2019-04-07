//
// Created by 曹顺 on 2019/2/23.
//

#include "module.h"
#include "string.h"

namespace hypermind {
    HM_OBJ_HASH(Module) {
        return 0;
    }
    HM_OBJ_FREE(Module) {
        return false;
    }
    HM_OBJ_DUMP(Module) {
        os << _HM_C(" { HMModule(") << sizeof(HMModule) << _HM_C(") ") << static_cast<const void *>(this) << " name : ";
        if (moduleName != nullptr) {
            moduleName->dump(os);
        } else {
            os << _HM_C("<core>");
        }
        os << std::endl;

        for (int i = 0; i < varValues.count; ++i) {
            String str(varNames.mSymbols[i].name, varNames.mSymbols[i].length);
            os << "   " << str << "  :  ";
            varValues[i].dump(os);
            os << std::endl;
        }

        os << _HM_C(" } ");

    }

    Value &HMModule::getVar(const Signature &signature) {
        HMInteger index = varNames.Find(signature);
        return varValues[index];
    }

}