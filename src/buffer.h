//
// Created by 曹顺 on 2019/2/13.
//

#ifndef HYPERMIND_BUFFER_H
#define HYPERMIND_BUFFER_H

#include <iostream>
#include "hypermind.h"
#include "vm.h"
// Buffer
#define BUFFER_GROWTH(count) CeilToPowerOf2(count)

namespace hypermind {
    HMUINT32 CeilToPowerOf2(HMUINT32 v);

    template <typename T>
    class Buffer {
    public:
        explicit Buffer(VM *mVM) : mVM(mVM) {}
        ~Buffer();

        void Fill(const T &data, HMUINT32 count);
        HMUINT32 Append(const T &data);
        HMUINT32 GetCount();
        T* GetBuffer();
        /**
         *  注意: 删除时会调用析构函数
         * @param index
         * @return
         */
        bool Remove(HMUINT32 index);
        bool Insert(HMUINT32 index, const T &data);
        void Dump();

        HMUINT32 GetSize() {
            return mCount * sizeof(T);
        };

        void Clear();

        T &operator[](HMUINT32 index);

    private:
        T *mData{nullptr};
        uint32_t mCount{0};
        uint32_t mCapacity{0};
        VM *mVM{};
        inline void EnsureCapacity(HMUINT32 count);

    };

    template<typename T>
    void Buffer<T>::Fill(const T &data, HMUINT32 count) {
        EnsureCapacity(mCount + count);
        for (int i = 0; i < count; ++i) {
            mData[mCount++] = data;
        }
    }

    template<typename T>
    HMUINT32 Buffer<T>::Append(const T &data) {
        Fill(data, 1);
        return mCount - 1;
    }

    template<typename T>
    T &Buffer<T>::operator[](HMUINT32 index) {
        if (index >= mCount)
            EnsureCapacity(mCount = index + 1);
        return mData[index];
    }

    template<typename T>
    void Buffer<T>::Clear() {
        mData = (T *) mVM->MemManger(mData, mCapacity * sizeof(T), 0);
    }

    template<typename T>
    Buffer<T>::~Buffer() {
        Clear();
    }

    /**
     *  确保容量 不足就扩大
     * @tparam T
     * @param count
     */
    template<typename T>
    inline void Buffer<T>::EnsureCapacity(HMUINT32 count) {
        if (count > mCapacity) {
            size_t oldSize = mCapacity * sizeof(T);
            mCapacity = BUFFER_GROWTH(count);
            size_t newSize = mCapacity * sizeof(T);
            mData = (T *) mVM->MemManger(mData, oldSize, newSize);
        }
    }

    template<typename T>
    HMUINT32 Buffer<T>::GetCount() {
        return mCount;
    }

    template<typename T>
    inline T *Buffer<T>::GetBuffer() {
        return mData;
    }

    template<typename T>
    bool Buffer<T>::Remove(HMUINT32 index) {
        // 删除一个对象
        mData[index].~T(); // 调用析构函数
        memcpy(&mData[index], &mData[index  + 1], (--mCount - index) * sizeof(T));
        return true;
    }

    template<typename T>
    bool Buffer<T>::Insert(HMUINT32 index, const T &data) {
        if (index >= mCount)
            EnsureCapacity(mCount = index + 1);
        else {
            EnsureCapacity(++mCount);
            memmove(&mData[index + 1], &mData[index], (mCount - index - 1) * sizeof(T));
        }
        mData[index] = data;
        return true;
    }

    template<typename T>
    void Buffer<T>::Dump() {
        std::cout << " -- buf dump -- " << std::endl;
        for (int i = 0; i < mCount; ++i) {
            std::cout << (T) mData[i] << std::endl;
        }
        std::cout << " -- end dump -- " << std::endl;
    }

}


#endif //HYPERMIND_BUFFER_H
