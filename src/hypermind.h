//
// Created by 曹顺 on 2019/2/11.
//

#ifndef HYPERMIND_HYPERMIND_H
#define HYPERMIND_HYPERMIND_H

#include <string>
#include <cstdio>
#include <cstdint>
// 字符宏

#define _HM_C(ch) ch
// 字符通用字符类型
typedef char HMChar;
// 数据类型
typedef char HMByte;
typedef int HMInteger;
typedef uint32_t HMUINT32;
typedef double HMDouble;
typedef float HMFloat;
typedef std::string String;
typedef HMUINT32 HMHash;

#define UNUSED __attribute__ ((unused))

#ifdef DEBUG
    #define ASSERT(condition, err) \
        do { \
            if (!(condition)) { \
                fprintf(stderr, "断言失败 !  %s:%d  in %s -> Message: %s", __FILE__, __LINE__, __func__, err); \
            } \
        } while (0);
#else
    #define ASSERT(condition, err) ((void) 0)
#endif

#endif //HYPERMIND_HYPERMIND_H
