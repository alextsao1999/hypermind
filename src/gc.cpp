//
// Created by 曹顺 on 2019/4/2.
//

#include "buffer.h"
#include "gc.h"
#include "obj/object.h"
#include "obj/class.h"
#include "obj/function.h"
#include "obj/string.h"
#include "obj/value.h"
#include "vm.h"
//#include "obj/class.h"
namespace hypermind {
    void GCHeap::Gray(HMObject *obj) {
        if (obj == nullptr || obj->isDark)
            return;
        obj->isDark = true;
        mGrayObjs.push_back(obj);
    }

    void *GCHeap::MemManger(void *ptr, size_t oldSize, size_t newSize) {
        mAllocatedBytes += newSize - oldSize;
        if (newSize == 0){
            free(ptr);
            return nullptr; // 防止野指针
        }
        return realloc(ptr, newSize);
    }

    void GCHeap::PushProtectedObject(HMObject *object) {
        ASSERT(mProtectedObjNum < MAX_PROTECTED_OBJ, "no more protected objs");
        mProtectedObj[mProtectedObjNum++] = object;
    }

    void GCHeap::PopProtectedObject() {
        mProtectedObjNum--;
    }

    void GCHeap::StartGC(VM *vm) {
        mSweepingBytes = 0;

        // 将要保护的对象置灰
        Gray(vm->mAllModule);
        for (int i = 0; i < mProtectedObjNum; ++i) {
            Gray(mProtectedObj[i]);
        }
        Gray(vm->mCurrentThread);

        // 将所有灰对象标黑
        BlackGrayObjects();

        HMObject *obj = mAllObjects;
        while (obj != nullptr) {
            if (obj->isDark) {
                // 黑对象置白
                obj->isDark = false;
            } else {
                HMObject *next;
                next = obj->next;

                // 白对象清除
                obj->free(vm);
//                vm->Deallocate(obj, sizeof(HMObject));
            }
        }
    }

    void GCHeap::Black(HMObject *obj) {
        switch (obj->type) {
            case ObjectType::Class:
                BlackClass((HMClass *) obj);
                break;
            case ObjectType::List:

                break;
            case ObjectType::Map:
                BlackMap((HMMap *) obj);
                break;
            case ObjectType::Module:
                BlackModule((HMModule *) obj);
                break;
            case ObjectType::Range:

                break;
            case ObjectType::String:
                BlackString((HMString *) obj);
                break;
            case ObjectType::Upvalue:

                break;
            case ObjectType::Function:
                BlackFunction((HMFunction *) obj);
                break;
            case ObjectType::Closure:
                BlackClosure((HMClosure *) obj);
                break;
            case ObjectType::Instance:
                BlackInstance((HMInstance *) obj);
                break;
            case ObjectType::Thread:

                break;
        }
    }

    void GCHeap::Gray(Value obj) {
        if (obj.type == ValueType::Object) {
            Gray(obj.objval);
        }
    }

    void GCHeap::Gray(Buffer<Value> values) {
        for (int i = 0; i < values.count; ++i) {
            Gray(values[i]);
        }
    }

    void GCHeap::BlackClass(HMClass *clazz) {
        Gray(clazz->classObj);
        Gray(clazz->superClass);
        Gray(clazz->name);
        for (int i = 0; i < clazz->methods.count; ++i) {
            if (clazz->methods[i].type == MethodType::Script) {
                Gray(clazz->methods[i].fn);
            }
        }
        mSweepingBytes += sizeof(HMClass) + clazz->methods.size();
    }

    void GCHeap::BlackMap(HMMap *map) {
        Gray(map->classObj);
        for (int i = 0; i < map->capacity; ++i) {
            Entry *entry = &map->entries[i];
            //跳过无效的entry
            if (entry->key.type != ValueType::Undefined) {
                Gray(entry->key);
                Gray(entry->value);
            }
        }
        mSweepingBytes += sizeof(HMMap);
        mSweepingBytes += sizeof(Entry) * map->capacity;
    }

    void GCHeap::BlackClosure(HMClosure *closure) {
        Gray(closure->pFn);
        for (int i = 0; i < closure->pFn->upvalueNum; ++i) {
            Gray(closure->upvalues[i]);
        }
        mSweepingBytes += sizeof(HMClosure);
        mSweepingBytes += sizeof(HMUpvalue *) * closure->pFn->upvalueNum;
    }

    void GCHeap::BlackFunction(HMFunction *function) {

    }

    void GCHeap::BlackModule(HMModule *module) {

    }

    void GCHeap::BlackInstance(HMInstance *instance) {

    }

    void GCHeap::BlackString(HMString *string) {

    }

}
