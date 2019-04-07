//
// Created by 曹顺 on 2019/2/11.
//

#ifndef HYPERMIND_HYPERMIND_H
#define HYPERMIND_HYPERMIND_H

#include <vector>
#include <string>
#include <cstdio>
#include <cstdint>

#define HMDebug
// 最大标识符长度
#define MAX_IDENTIFIER_LENTH 255
// 最大局部变量个数
#define MAX_LOCAL_VAR_NUMBER 128
#define MAX_UPVALUE_NUMBER 128

// 数据类型
typedef char HMByte;
typedef int HMInteger;
typedef uint32_t HMUINT32;
typedef double HMDouble;
typedef float HMFloat;
typedef HMUINT32 HMHash;
typedef bool HMBool;
#define Vector std::vector
#define HMUNICODE

#ifdef HMUNICODE
// Unicode
// 字符通用字符类型
#define _HM_C(ch) L##ch
typedef wchar_t HMChar;
typedef std::wstring String;
typedef std::wostream Ostream;
#define hm_cout std::wcout
#define hm_memcpy wmemcpy
#define hm_memcmp wmemcmp
#define hm_strtoi _wtoi
#define hm_strlen wcslen
#else
// ---- Ansi ----
#define _HM_C(ch) ch
#include <cstring>
typedef char HMChar;
typedef std::string String;
typedef std::ostream Ostream;
#define hm_cout std::cout
#define hm_memcpy memcpy
#define hm_memcmp memcmp
#define hm_strtoi atoi
#define hm_strlen strlen
#endif

namespace hypermind {
    struct HMClass;
    struct HMObject;
    struct HMClosure;
}
#define UNUSED __attribute__ ((unused))

#define MEMRY_ERROR(C)

#ifdef HMDebug
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
