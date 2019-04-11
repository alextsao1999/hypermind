//
// Created by 曹顺 on 2019/2/24.
//

#include "map.h"

namespace hypermind {
    HM_OBJ_DUMP(Map) {
        os << _HM_C(" { HMMap(") << sizeof(HMMap) << _HM_C(") ") << static_cast<const void *>(this) <<
        _HM_C("  count : ") << count << _HM_C("    ");

        if (capacity > 0) {
            Entry *entryArr = entries;
            HMInteger idx = 0;
            while (idx < capacity) {
                if (entryArr[idx].key.type != ValueType::Undefined) {
                    entryArr->key.dump(hm_cout);
                    hm_cout << "->";
                    entryArr->value.dump(hm_cout);
                    hm_cout << "   ";
                }
                idx++;
            }
        }

        hm_cout << _HM_C(" } ");

    }
    HM_OBJ_HASH(Map) {
        return 0;
    }
    HM_OBJ_FREE(Map) {
        vm->Deallocate(entries, sizeof(Entry) * capacity);
        HM_FREE_THIS(Map);
    }

    HMBool addEntry(Entry *entries, HMUINT32 capacity, Value key, Value value) {
        uint32_t index = key.hash() % capacity;
        while (true) {
            if (entries[index].key.type == ValueType::Undefined) {
                entries[index].key = key;
                entries[index].value = value;
                return true;       //新的key就返回true
            } else if (entries[index].key.equals(key)) { //key已经存在,仅仅更新值就行
                entries[index].value = value;
                return false;    // 未增加新的key就返回false
            }
            //开放探测定址,尝试下一个slot
            index = (index + 1) % capacity;
        }

    }
}