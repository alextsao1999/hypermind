//
// Created by 曹顺 on 2019/2/13.
//

#include "vm.h"

namespace hypermind {
    HMClass* Value::getClass(const hypermind::VM *vm) const {
        switch (type) {
            case ValueType::Integer:
                return vm->mIntegerClass;
            case ValueType::Float:
                return vm->mFloatClass;
            case ValueType::Undefined:
            case ValueType::Null:
                break;
            case ValueType::Object:
                return objval->classObj;
            case ValueType::True:
            case ValueType::False:
                return vm->mBooleanClass;
            default:
                break;
        }
        return vm->mMetaClass;
    };

    void *VM::MemManger(void *ptr, size_t oldSize, size_t newSize) {
        mAllocatedBytes += newSize - oldSize;
        if (newSize == 0){
            free(ptr);
            return nullptr; // 防止野指针
        }
        return realloc(ptr, newSize);
    }

    HMUINT32 VM::GetAllocatedBytes() {
        return mAllocatedBytes;
    }

}
