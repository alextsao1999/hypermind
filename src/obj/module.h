//
// Created by 曹顺 on 2019/2/23.
//

#ifndef HYPERMIND_MODULE_H
#define HYPERMIND_MODULE_H

#include "buffer.h"
#include "object.h"
#include "value.h"
#include "string.h"
#include <symbol.h>

namespace hypermind {
    // 模块对象
    HM_OBJECT(Module) {
        HMString *moduleName;
        SymbolTable varNames;
        Buffer<Value> varValues;
        HM_OBJ_CONSTRUCTOR_CLASS(Module, nullptr, HMString *name), moduleName(name) {};

        HM_OBJ_DECL();
    };

}

#endif //HYPERMIND_MODULE_H
