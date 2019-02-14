//
// Created by 曹顺 on 2019/2/13.
//

#ifndef HYPERMIND_BUFFER_H
#define HYPERMIND_BUFFER_H
#include <vm.h>
#include <hypermind.h>
#include <cstdint>

// Buffer
#define BUFFER_GROWTH(count) ceilToPowerOf2(count)

namespace hypermind {

    uint32_t ceilToPowerOf2(uint32_t v);

    template <class Type>
    class Buffer {
    public:
        explicit Buffer(VM *mVM) : mVM(mVM) {}
        ~Buffer();

        void fill(Type data, uint32_t count);
        void add(Type data);
        uint32_t getCount();
        Type* getData();

        void clear();

        Type &operator[](uint32_t index);


    private:
        Type *mData{nullptr};
        uint32_t mCount{0};
        uint32_t mCapacity{0};
        VM *mVM;
        inline void enlarge(uint32_t count);

    };

    template<class Type>
    void Buffer<Type>::fill(Type data, uint32_t count) {
        enlarge(mCount + count);
        for (int i = 0; i < count; ++i) {
            mData[mCount++] = data;
        }
    }

    template<class Type>
    void Buffer<Type>::add(Type data) {
        fill(data, 1);
    }

    template<class Type>
    Type &Buffer<Type>::operator[](uint32_t index) {
        if (index > mCount) {
            enlarge(index + 1);
            mCount = index + 1;
        }
        return mData[index];
    }

    template<class Type>
    void Buffer<Type>::clear() {
        mData = (Type *) mVM->memManger(mData, mCapacity * sizeof(Type), 0);
    }

    template<class Type>
    Buffer<Type>::~Buffer() {
        clear();
    }

    /**
     *  确保容量 不足就扩大
     * @tparam Type
     * @param count
     */
    template<class Type>
    inline void Buffer<Type>::enlarge(uint32_t count) {
        if (count > mCapacity) {
            size_t oldSize = mCapacity * sizeof(Type);
            mCapacity = BUFFER_GROWTH(count);
            size_t newSize = mCapacity * sizeof(Type);
            mData = (Type *) mVM->memManger(mData, oldSize, newSize);
        }
    }

    template<class Type>
    uint32_t Buffer<Type>::getCount() {
        return mCount;
    }

    template<class Type>
    Type *Buffer<Type>::getData() {
        return mData;
    }

}


#endif //HYPERMIND_BUFFER_H
