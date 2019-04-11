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
#include "obj/thread.h"
#include "compiler.h"
#include "vm.h"
//#include "obj/class.h"
namespace hypermind {
    void *GCHeap::MemManger(void *ptr, size_t oldSize, size_t newSize) {
        mAllocatedBytes += newSize - oldSize;
        if (newSize == 0){
            free(ptr);
            return nullptr; // 防止野指针
        }

        if (mAllocatedBytes > mNextGC) {
            StartGC();
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

    void GCHeap::StartGC() {
        mScaningBytes = 0;

        // 将要保护的对象置灰
        Gray(mVM->mAllModule);
        for (int i = 0; i < mProtectedObjNum; ++i) {
            Gray(mProtectedObj[i]);
        }
        Gray(mVM->mCurrentThread);

        // 将所有灰对象标黑
        while (!mGrayObjs.empty()) {
            HMObject *obj = mGrayObjs.back();
            mGrayObjs.pop_back();
            Black(obj);
        }

        printf("GC :  sweeping : %d allocated : %d \n", mScaningBytes, mAllocatedBytes);

        HMObject **obj = &mAllObjects;
        while (*obj != nullptr) {
            if ((*obj)->isDark) {
                // 黑对象保留 置白
                (*obj)->isDark = false;
                obj = &((*obj)->next);
            } else {
                // 白对象 清除
                HMObject *unreached = *obj;
                *obj = unreached->next;
                unreached->free(mVM);
            }
        }

        mNextGC = static_cast<HMUINT32>(mAllocatedBytes * mHeapGrowthFactor);
        if (mNextGC < mMinHeapSize) {
            mNextGC = mMinHeapSize;
        }

    }

    void GCHeap::Gray(Value obj) {
        if (obj.type == ValueType::Object) {
            Gray(obj.objval);
        }
    }

    void GCHeap::Gray(HMObject *obj) {
        if (obj == nullptr || obj->isDark)
            return;
        obj->isDark = true;
        mGrayObjs.push_back(obj);
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
                BlackUpvalue((HMUpvalue *) obj);
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
                BlackThread((HMThread *) obj);
                break;
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
        mScaningBytes += sizeof(HMClass) + clazz->methods.size();
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
        mScaningBytes += sizeof(HMMap);
        mScaningBytes += sizeof(Entry) * map->capacity;
    }

    void GCHeap::BlackClosure(HMClosure *closure) {
        Gray(closure->classObj);
        Gray(closure->pFn);
        for (int i = 0; i < closure->pFn->upvalueNum; ++i) {
            Gray(closure->upvalues[i]);
        }
        mScaningBytes += sizeof(HMClosure);
        mScaningBytes += sizeof(HMUpvalue *) * closure->pFn->upvalueNum;
    }

    void GCHeap::BlackFunction(HMFunction *function) {
        Gray(function->classObj);
        for (int i = 0; i < function->constants.count; ++i) {
            Gray(function->constants[i]);
        }
        mScaningBytes += sizeof(HMFunction);
        mScaningBytes +=
                function->instructions.size() + function->constants.size() + function->symbols.mSymbols.size() +
                function->upvalueNum * sizeof(Upvalue);
#ifdef HMDebug
        mScaningBytes += sizeof(FunctionDebug) + function->debug->line.size();
#endif

    }

    void GCHeap::BlackModule(HMModule *module) {
        Gray(module->moduleName);
        for (int i = 0; i < module->varValues.count; ++i) {
            Gray(module->varValues[i]);
        }
        mScaningBytes += sizeof(HMModule);
        mScaningBytes += module->varNames.mSymbols.size() + module->varValues.size();
    }

    void GCHeap::BlackInstance(HMInstance *instance) {
        Gray(instance->classObj);
        for (int i = 0; i < instance->classObj->fieldNubmer; ++i) {
            Gray(instance->fields[i]);
        }
        mScaningBytes += sizeof(HMInstance);
        mScaningBytes += sizeof(Value) * instance->classObj->fieldNubmer;
    }

    void GCHeap::BlackString(HMString *string) {
        Gray(string->classObj);
        mScaningBytes += sizeof(HMString) + (string->length + 1) * sizeof(HMChar);
    }

    void GCHeap::BlackThread(HMThread *thread) {
        mScaningBytes += sizeof(HMThread);
        mScaningBytes += thread->stackCapacity * sizeof(Value) + thread->frameCapacity * sizeof(Frame);

    }

    void GCHeap::BlackUpvalue(HMUpvalue *upvalue) {
        Gray(upvalue->closedUpvalue);
        mScaningBytes += sizeof(HMUpvalue);
    }

    GCHeap::GCHeap(VM *vm) : mVM(vm) {
    }

}
