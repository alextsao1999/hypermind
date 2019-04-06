//
// Created by 曹顺 on 2019/2/13.
//

#ifndef HYPERMIND_BUFFER_H
#define HYPERMIND_BUFFER_H

#include <iostream>
#include "gc.h"
// Buffer
#define BUFFER_GROWTH(count) CeilToPowerOf2(count)

namespace hypermind {
    HMUINT32 CeilToPowerOf2(HMUINT32 v);

    template <typename T>
    struct Buffer {
        T *data{nullptr};
        HMUINT32 count{0};
        HMUINT32 capacity{0};

        inline void fill(GCHeap *heap, const T &value, HMUINT32 num) {
            ensureCapacity(heap, count + num);
            for (int i = 0; i < num; ++i) {
                data[count++] = value;
            }
        };

        inline HMUINT32 append(GCHeap *heap, const T &data) {
            fill(heap, data, 1);
            return count - 1;
        };

        /**
         *  注意: 删除时会调用析构函数
         * @param index
         * @return
         */
        inline bool remove(HMUINT32 index) {
            memcpy(&data[index], &data[index  + 1], (--count - index) * sizeof(T));
            return true;
        };

        inline bool insert(GCHeap *heap, HMUINT32 index, const T &value) {
            if (index >= count)
                ensureCapacity(heap, count = index + 1);
            else {
                ensureCapacity(heap, ++count);
                memmove(&data[index + 1], &data[index], (count - index - 1) * sizeof(T));
            }
            data[index] = value;
            return true;
        };

        inline void set(GCHeap *heap, HMUINT32 index, const T &value) {
            if (index >= count)
                ensureCapacity(heap, count = index + 1);
            data[index] = value;
        };

        inline HMUINT32 size() {
            return capacity * sizeof(T);
        };

        inline void clear(GCHeap *heap) {
            data = (T *) heap->MemManger(data, capacity * sizeof(T), 0);
        }

        void dump() {
            std::cout << " -- buf dump -- " << std::endl;
            for (int i = 0; i < count; ++i) {
                std::cout << data[i] << std::endl;
            }
            std::cout << " -- end dump -- " << std::endl;
        };

        /**
         * 确保容量
         * @param mVM
         * @param count
         */
        inline void ensureCapacity(GCHeap *heap, HMUINT32 newCapacity) {
            if (newCapacity > capacity) {
                size_t oldSize = capacity * sizeof(T);
                capacity = BUFFER_GROWTH(newCapacity);
                data = (T *) heap->MemManger(data, oldSize, capacity * sizeof(T));
            }
        };

        /**
         * 缩小到合适大小
         * @param heap
         */
        void shrink(GCHeap *heap) {
            if (capacity > count) {
                data = (T *) heap->MemManger(data, count * sizeof(T), 0);
                capacity = count;
            }
        }

        inline T &operator[](const HMInteger &index) {
            return data[index];
        }

    };

}


#endif //HYPERMIND_BUFFER_H
