//
// Created by 曹顺 on 2019/2/13.
//

#ifndef HYPERMIND_BUFFER_H
#define HYPERMIND_BUFFER_H

#include <hypermind.h>

namespace hypermind {
    template <typename Type>
    class Buffer {
    public:

    private:
        Type *mData;
        size_t mCount;
        size_t mCapacity;


    };
}


#endif //HYPERMIND_BUFFER_H
