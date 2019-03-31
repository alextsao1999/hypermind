//
// Created by 曹顺 on 2019/2/24.
//

#ifndef HYPERMIND_MAP_H
#define HYPERMIND_MAP_H

#include "vm.h"
#include "value.h"
#define INITCAPACITY 5019
namespace hypermind {
    struct Entry {
        HMHash hash{0}; // 将hash值储存避免重复计算
        Value key;
        Value value;
    };

    HM_OBJECT(Map) {
        Entry *entries{nullptr};
        HMUINT32 count;
        HMUINT32 capacity;

        void resize(VM *vm, HMUINT32 newCapacity) {
            auto *newEntries = vm->Allocate<Entry>(newCapacity);
            uint32_t idx = 0;
            while (idx < newCapacity) {
                new(&newEntries[idx].key) Value();
                new(&newEntries[idx].value) Value(false);
                idx++;
            }
        }
        HM_OBJ_DECL();
    };

/*
    static bool addEntry(Entry *entries, uint32_t capacity, Value key, Value value) {
        uint32_t index = key.hash() % capacity;

        //通过开放探测法去找可用的slot
        while (true) {
            //找到空闲的slot,说明目前没有此key,直接赋值返回
            if (entries[index].key.type == ValueType::Undefined) {
                entries[index].key = key;
                entries[index].value = value;
                return true;       //新的key就返回true
            } else if (valueIsEqual(entries[index].key, key)) { //key已经存在,仅仅更新值就行
                entries[index].value = value;
                return false;    // 未增加新的key就返回false
            }

            //开放探测定址,尝试下一个slot
            index = (index + 1) % capacity;
        }
    }
*/



}

#endif //HYPERMIND_MAP_H
