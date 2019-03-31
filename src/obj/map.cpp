//
// Created by 曹顺 on 2019/2/24.
//

#include "map.h"

namespace hypermind {
    HM_OBJ_DUMP(Map) {

    }
    HM_OBJ_HASH(Map) {
        return 0;
    }
    HM_OBJ_FREE(Map) {
        return false;
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