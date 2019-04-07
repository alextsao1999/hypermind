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
        FunctionCall  // 有关函数对象的调用方法, 用来实现函数重载
    };

    // Method对象
    struct HMMethod {
        MethodType type;
        union {
            HMClosure *fn{};  // 方法引用的函数闭包
            HMPrimitive pfn; // 指向原生方法
        };
        HMMethod(HMPrimitive func) : type(MethodType::Primitive), pfn(func) {}

        HMMethod(HMClosure *func) : type(MethodType::Script), fn(func) {}

        HMMethod(MethodType type) : type(type), fn(nullptr) {}
    };

    // 类对象
    HM_OBJECT(Class) {
        HMClass *superClass;  // 父类
        HMUINT32 fieldNubmer;
        Buffer<HMMethod> methods;
        SymbolTable methodTable;
        HMString *name; // 类名
        HM_OBJ_CONSTRUCTOR_CLASS(Class, nullptr, HMString *name, HMClass *super, HMUINT32 fieldNumber), name(name),
                                                                                                        superClass(super),
                                                                                                        fieldNubmer(fieldNumber){
            if (super != nullptr) {
                // 继承super的方法
                for (int i = 0; i < super->methods.count; ++i) {
                    methods.append(&vm->mGCHeap, super->methods[i]);
                }
            }
            // 加到保护对象中?
        };

        inline void bind(VM *vm, Signature signature, HMPrimitive func) {
            methods.set(&vm->mGCHeap, static_cast<HMUINT32>(vm->mAllMethods.EnsureFind(&vm->mGCHeap, signature)),
                        HMMethod(func));
        }

        inline void bind(VM *vm, Signature signature, HMMethod method) {
            methods.set(&vm->mGCHeap, static_cast<HMUINT32>(vm->mAllMethods.EnsureFind(&vm->mGCHeap, signature)),
                        method);
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
