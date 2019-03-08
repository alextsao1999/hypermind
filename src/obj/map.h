//
// Created by 曹顺 on 2019/2/24.
//

#ifndef HYPERMIND_MAP_H
#define HYPERMIND_MAP_H

#include "object.h"
#include "value.h"
#define INITCAPACITY 5019
namespace hypermind {
    HM_OBJECT(Map) {
        struct Node {
            HMHash hash; // 将hash值储存
            Value *key;
            Value *value;
            Node *next; // 拉链法 链接下一个Entry
        };
        Value *entries;
        HMUINT32 count;

        HM_OBJ_DECL();
    };

}

#endif //HYPERMIND_MAP_H
