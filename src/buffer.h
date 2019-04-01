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
    struct Buffer {
        T *data{nullptr};
        HMUINT32 count{0};
        HMUINT32 capacity{0};

        inline void fill(VM *vm, const T &value, HMUINT32 num) {
            ensureCapacity(vm, count + num);
            for (int i = 0; i < num; ++i) {
                data[count++] = value;
            }
        };

        inline HMUINT32 append(VM *vm, const T &data) {
            fill(vm, data, 1);
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

        inline bool insert(VM *vm, HMUINT32 index, const T &value) {
            if (index >= count)
                ensureCapacity(vm, count = index + 1);
            else {
                ensureCapacity(vm, ++count);
                memmove(&data[index + 1], &data[index], (count - index - 1) * sizeof(T));
            }
            data[index] = value;
            return true;
        };

        inline HMUINT32 size() {
            return capacity * sizeof(T);
        };

        inline void clear(VM *vm) {
            data = (T *) vm->MemManger(data, capacity * sizeof(T), 0);
        }

        void dump() {
            std::cout << " -- buf dump -- " << std::endl;
            for (int i = 0; i < count; ++i) {
                std::cout << (T) data[i] << std::endl;
            }
            std::cout << " -- end dump -- " << std::endl;
        };

        /**
         * 确保容量
         * @param mVM
         * @param count
         */
        inline void ensureCapacity(VM *vm, HMUINT32 newCapacity) {
            if (newCapacity > capacity) {
                size_t oldSize = capacity * sizeof(T);
                capacity = BUFFER_GROWTH(newCapacity);
                data = (T *) vm->MemManger(data, oldSize, capacity * sizeof(T));
            }
        };

        /**
         * 缩小到合适大小
         * @param mVM
         */
        void shrink(VM *vm) {
            if (capacity > count) {
                data = (T *) vm->MemManger(data, count * sizeof(T), 0);
                capacity = count;
            }
        }

        inline T &operator[](const HMInteger &index) {
            return data[index];
        }

    };

}


#endif //HYPERMIND_BUFFER_H
