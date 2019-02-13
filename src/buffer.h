//
// Created by 曹顺 on 2019/2/13.
//

#ifndef HYPERMIND_BUFFER_H
#define HYPERMIND_BUFFER_H

#include <vm.h>
#include <hypermind.h>
#include <cstdint>
namespace hypermind {

    uint32_t ceilToPowerOf2(uint32_t v);


    template <class Type>
    class Buffer {
    public:
        explicit Buffer(VM *mVM) : mVM(mVM) {}
        void fill(Type data, uint32_t count);
        void add(Type data);

        Type &operator[](uint32_t index);

    private:
        Type *mData{nullptr};
        uint32_t mCount{0};
        uint32_t mCapacity{0};
        VM *mVM;

    };

    template<class Type>
    void Buffer<Type>::fill(Type data, uint32_t count) {
        uint32_t newCount = mCount + count;
        if (newCount > mCapacity) {
            uint32_t oldSize = mCapacity * sizeof(Type);
            mCapacity = ceilToPowerOf2(newCount);
            uint32_t newSize = mCapacity * sizeof(Type);
            mData = (Type *) mVM->memManger(mData, oldSize, newSize);
        }
        for (int i = 0; i < count; ++i) {
            mData[count++] = data;
        }
    }

    template<class Type>
    void Buffer<Type>::add(Type data) {
        fill(data, 1);
    }

    template<class Type>
    Type &Buffer<Type>::operator[](uint32_t index) {
        return mData[index];
    }


}


#endif //HYPERMIND_BUFFER_H
