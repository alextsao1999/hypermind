//
// Created by 曹顺 on 2019/2/24.
//

#ifndef HYPERMIND_MAP_H
#define HYPERMIND_MAP_H

#define CAPACITY_GROW_FACTOR 4
#define MIN_CAPACITY 64
#define MAP_LOAD_PERCENT 0.8

#include "vm.h"
#include "value.h"
namespace hypermind {
    struct Entry {
        HMHash hash{0}; // 将hash值储存避免重复计算
        Value key;
        Value value;
    };

    HMBool addEntry(Entry *entries, HMUINT32 capacity, Value key, Value value);

    HM_OBJECT(Map) {
        Entry *entries{nullptr};
        HMUINT32 count{0};
        HMUINT32 capacity{0};

        HM_OBJ_CONSTRUCTOR(Map) {}

        void resize(VM *vm, HMUINT32 newCapacity) {
            auto *newEntries = vm->Allocate<Entry>(newCapacity);
            uint32_t idx = 0;
            while (idx < newCapacity) {
                newEntries[idx].key = Value();
                newEntries[idx].value = Value(false);
//                new(&newEntries[idx].value) Value(false);
                idx++;
            }
            if (capacity > 0) {
                Entry *entryArr = entries;
                idx = 0;
                while (idx < capacity) {
                    if (entryArr[idx].key.type != ValueType::Undefined) {
                        addEntry(newEntries, newCapacity, entryArr[idx].key, entryArr[idx].value);
                    }
                    idx++;
                }
            }
            vm->Deallocate(entries, count * sizeof(Entry));
            entries = newEntries;
            capacity = newCapacity;

        }

        Entry *find(Value key) {
            if (capacity == 0) {
                return nullptr;
            }
            HMUINT32 index = key.hash() % capacity;
            Entry *entry;
            while (true) {
                entry = &entries[index];
                if (entry->key.equals(key)) {
                    return entry;
                }
                if (entry->key.type == ValueType::Undefined && entry->value.type == ValueType::False) {
                    return nullptr;
                }
                index = (index + 1) % capacity;
            }
        }

        void set(VM *vm, Value key, Value value) {
            if (count + 1 > capacity * MAP_LOAD_PERCENT) {
                HMUINT32 newCapacity = capacity * CAPACITY_GROW_FACTOR;
                if (newCapacity < MIN_CAPACITY) {
                    newCapacity = MIN_CAPACITY;
                }
                resize(vm, newCapacity);
            }
            if (addEntry(entries, capacity, key, value)) {
                count++;
            }
        }

        Value get(Value key) {
            Entry *entry = find(key);
            if (entry == nullptr) {
                return {};
            }
            return entry->value;
        }

        void clear(VM *vm) {
            vm->Deallocate(entries, sizeof(Entry) * count);
            entries = nullptr;
            capacity = count = 0;
        }

        Value remove(VM *vm, Value key) {
            Entry *entry = find(key);
            if (entry == nullptr) {
                return {};
            }
            Value value = entry->value;
            entry->key = Value();
            entry->value = Value(true);
            if (value.type == ValueType::Object) {
                vm->mGCHeap.PushProtectedObject(value.objval);
            }
            count--;
            if (count == 0) { //若删除该entry后map为空就回收该空间
                clear(vm);
            } else if (count < capacity / (CAPACITY_GROW_FACTOR) * MAP_LOAD_PERCENT &&
                       count > 64) {   //若map容量利用率太低,就缩小map空间
                uint32_t newCapacity = capacity / CAPACITY_GROW_FACTOR;
                if (newCapacity < MIN_CAPACITY) {
                    newCapacity = MIN_CAPACITY;
                }
                resize(vm, newCapacity);
            }
            if (value.type == ValueType::Object) {
                vm->mGCHeap.PopProtectedObject();
            }

            return value;
        }

        HM_OBJ_DECL();
    };

}

#endif //HYPERMIND_MAP_H
