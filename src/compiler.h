//
// Created by 曹顺 on 2019/2/25.
//

#ifndef HYPERMIND_COMPILER_H
#define HYPERMIND_COMPILER_H

#include "vm.h"
namespace hypermind {
    class CompilerUnit {
    public:
        explicit CompilerUnit(VM *mVm);

    protected:
        VM *mVm;


    };

}

#endif //HYPERMIND_COMPILER_H
