//
// Created by 曹顺 on 2019/2/19.
//

#ifndef HYPERMIND_OBJECT_H
#define HYPERMIND_OBJECT_H

#include <iostream>
#include "hypermind.h"
#include "vm.h"

#define HM_OBJECT(name) \
struct HM##name : public HMObject
#define HM_OBJ_DECL() using HMObject::HMObject; \
HMHash hash() override; \
void dump(Ostream &os) override; \
HMBool free(VM *vm) override;
#define HM_OBJ_HASH(obj)  HMHash HM##obj::hash()
#define HM_OBJ_DUMP(obj)  void HM##obj::dump(Ostream &os)
#define HM_OBJ_FREE(obj)  HMBool HM##obj::free(VM *vm)

namespace hypermind {
    struct HMClass;

    enum class ObjectType {
        Class,   //此项是class类型,以下都是object类型
        List,
        Map,
        Module,
        Range,
        String,
        Upval,
        Function,
        Method,
        Closure,
        Instance,
        Thread
    };
    // Object 是一切对象的基类
    struct HMObject {
        ObjectType type{};
        bool isDark{false};  // 是否可达
        HMClass *classObj{};
        HMObject *next{}; // 链接下一个Object

        HMObject() = default;
        HMObject(ObjectType type, HMClass *classObj) : type(type), classObj(classObj) {};

        /**
         * 自动将对象链接到虚拟机中
         * @param vm
         * @param type
         * @param classObj
         */
        HMObject(VM *vm, ObjectType type, HMClass *classObj);

        /**
         * 获得对象hash值
         * @return hash值
         */
        virtual HMHash hash() = 0;

        virtual void dump(Ostream &os){};

        /**
         * 释放对象内存
         * @return bool
         */
        virtual HMBool free(VM *vm){
            return true;
        };
    };


}
#endif //HYPERMIND_OBJECT_H
