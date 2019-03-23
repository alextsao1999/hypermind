//
// Created by 曹顺 on 2019/2/23.
//

#ifndef HYPERMIND_MODULE_H
#define HYPERMIND_MODULE_H

#include "buffer.h"
#include "object.h"
#include "value.h"
#include "string.h"

#define HM_OBJ_CONSTRUCTOR()

namespace hypermind {
    // 模块对象
    HM_OBJECT(Module) {
        HMString *mModuleName;

        Buffer<Value> mModuleVariables;
#ifdef HMDebug
        Vector<String> mModuleVarNames;
#endif
        HM_OBJ_CONSTRUTOR_CLASS(Module, nullptr, HMString *name), mModuleName(name), mModuleVariables(Buffer<Value>(vm)) {

        }

        HM_OBJ_DECL();
    };

}

#endif //HYPERMIND_MODULE_H
