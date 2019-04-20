//
// Created by 曹顺 on 2019/2/23.
//

#ifndef HYPERMIND_CLASS_H
#define HYPERMIND_CLASS_H

#include "buffer.h"
#include "function.h"
#include "value.h"
#include "string.h"

namespace hypermind {
    // 原生函数指针
    typedef bool (*HMPrimitive)(VM *vm, HMInteger argNum, Value *args);

    enum class MethodType {
        None,     // 空方法类型, 并不等同于undefined
        Primitive,   // 原生方法
        Script,  // 脚本中定义的方法
        FunctionCall,  // 有关函数对象的调用方法, 用来实现函数重载
        Getter,
        Setter,
    };

    // Method对象
    struct HMMethod {
        MethodType type;
        union {
            HMClosure *fn{};  // 方法引用的函数闭包
            HMPrimitive pfn; // 指向原生方法
            HMInteger index; // 指向域的索引
        };
        HMMethod(HMPrimitive func) : type(MethodType::Primitive), pfn(func) {}

        HMMethod(HMClosure *func) : type(MethodType::Script), fn(func) {}

        HMMethod(MethodType type) : type(type), fn(nullptr) {}

        HMMethod(MethodType type, HMInteger index) : type(type), index(index) {}
    };

    // 类对象
    HM_OBJECT(Class) {
        HMClass *superClass{nullptr};  // 父类
        HMUINT32 fieldNubmer;
        Buffer<HMMethod> methods;
        SymbolTable table;
        HMString *name; // 类名
        HM_OBJ_CONSTRUCTOR_CLASS(Class, nullptr, HMString *name, HMClass *super, HMUINT32 fieldNumber), name(name),
                                                                                                        superClass(super),
                                                                                                        fieldNubmer(fieldNumber){
            vm->mGCHeap.PushProtectedObject(this);
            HMChar newName[MAX_IDENTIFIER_LENTH] = {_HM_C('\0')};
            hm_memcpy(newName, name->charSequence, name->length);
            hm_memcpy(newName + name->length, " Class", 6);
            classObj = vm->NewObject<HMClass>(newName, name->length + 6, 0);
            vm->mGCHeap.PushProtectedObject(classObj); // 加入到保护对象中
            classObj->classObj = vm->mMetaClass; // 设置Class Meta类的类为 Class
            classObj->bindSuper(vm, vm->mMetaClass); // 继承Meta类的方法
            bindSuper(vm, super);
            vm->mGCHeap.PopProtectedObject();
            vm->mGCHeap.PopProtectedObject();
        };

        HM_OBJ_CONSTRUCTOR_CLASS(Class, nullptr, const HMChar *name, HMUINT32 length, HMUINT32 fieldNumber),
                name(vm->NewObject<HMString>(name, length)), fieldNubmer(fieldNumber) {
        };


        inline void bindSuper(VM *vm, HMClass *super) {
            if (super != nullptr) {
                fieldNubmer += super->fieldNubmer;
                // 继承super的方法
                for (int i = 0; i < super->methods.count; ++i) {
                    methods.push(&vm->mGCHeap, super->methods[i]);
                }
            }
        }

        inline void bind(VM *vm, Signature signature, HMMethod method) {
            methods.set(&vm->mGCHeap, static_cast<HMUINT32>(vm->mAllMethods.EnsureFind(&vm->mGCHeap, signature)),
                        method);
        }

        inline void bind(VM *vm, HMUINT32 index, HMMethod method) {
            methods.set(&vm->mGCHeap, index, method);
        }

        HM_OBJ_DECL();
    };

    // 实例对象
    HM_OBJECT(Instance) {
        Value *fields{nullptr};
        HM_OBJ_CONSTRUCTOR_CLASS(Instance, clazz, HMClass *clazz), fields(vm->Allocate<Value>(clazz->fieldNubmer)) {};
        HM_OBJ_DECL();
    };

}

#endif //HYPERMIND_CLASS_H
