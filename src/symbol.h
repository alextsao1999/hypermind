//
// Created by 曹顺 on 2019/4/1.
//

#ifndef HYPERMIND_SYMBOL_H
#define HYPERMIND_SYMBOL_H

#include "buffer.h"
#include "lexer.h"
#include "utils.h"
namespace hypermind {
    enum class SignatureType {
        None,
        Class,
        Method,
        Getter,
        Setter,
        Subscript,
        SubscriptSetter,
        Constructor
    };

    struct Signature {
        HMUINT32 length{0};
        HMHash hash{0};
        const HMChar *name{nullptr};
        SignatureType type{SignatureType::None};

        Signature(SignatureType type, const HMChar *str, HMUINT32 length) : type(type), name(str), length(length),
                                                                            hash(hashString(str, length)) {}

        Signature(SignatureType type, const HMChar *str) : type(type), name(str), length(hm_strlen(str)),
                                                                            hash(hashString(str, length)) {}

        Signature(SignatureType type, const HMChar *str, HMUINT32 length, const HMChar *field, HMUINT32 fieldLength)
                : type(type), name(str), length(length), hash(hashString(field, fieldLength)) {
            mixHashString(hash, _HM_C(" "), 1);
            mixHashString(hash, str, length);
        }

        Signature(const HMChar *str, HMUINT32 length, const HMChar *field, HMUINT32 fieldLength) : type(SignatureType::None), name(str), length(length),
                                                        hash(hashString(field, fieldLength)) {
            mixHashString(hash, _HM_C(" "), 1);
            mixHashString(hash, str, length);
        }

        Signature(const Token &token) : type(SignatureType::None), name(token.start), length(token.length) ,
                                        hash(hashString(token.start, token.length)) {}
        Signature(const HMChar *str) : type(SignatureType::None), name(str), length(hm_strlen(str)),
                                       hash(hashString(str, length)) {}

        explicit Signature(SignatureType type) : type(type), name(nullptr), length(0), hash(0) {}

        Signature() = default;

        bool operator==(const Signature&signature) {
            return type == signature.type && length == signature.length && hash == signature.hash &&
                   hm_memcmp(name, signature.name, length) == 0;
        }

        bool operator<(const Signature&signature) {
            return length < signature.length;
        }

        void dump(Ostream &os) const {
            switch (type) {
                case SignatureType::None:
                    break;
                case SignatureType::Class:
                    break;
                case SignatureType::Method:
                    os << _HM_C("[method]");
                    break;
                case SignatureType::Getter:
                    os << _HM_C("[getter]");
                    break;
                case SignatureType::Setter:
                    os << _HM_C("[setter]");
                    break;
                case SignatureType::Subscript:
                    os << _HM_C("[subscript]");
                    break;
                case SignatureType::SubscriptSetter:
                    os << _HM_C("[subscript setter]");
                    break;
                case SignatureType::Constructor:
                    os << _HM_C("[constructor]");
                    break;
            }
            String str(name, length);
            os << str;

        }
    };

    class SymbolTable {
    public:
        Buffer<Signature> mSymbols;

        HMInteger Add(GCHeap *heap, SignatureType type, HMChar *name, HMUINT32 length) {
            return mSymbols.push(heap, Signature(type, name, length));
        };

        HMInteger Add(GCHeap *heap, Signature signature) {
            return mSymbols.push(heap, signature);
        };

        /**
         * 找到返回索引 未找到添加后返回索引
         * @param heap
         * @param signature
         * @return
         */
        HMInteger EnsureFind(GCHeap *heap, Signature signature) {
            HMInteger index = Find(signature);
            if (index == -1) {
                return Add(heap, signature);
            }
            return index;
        };

        HMInteger Find(Signature signature) {
            for (int i = 0; i < mSymbols.count; ++i) {
                if (mSymbols[i] == signature) {
                    return i;
                }
            }
            return -1;
        };

        inline HMUINT32 GetCount() {
            return mSymbols.count;
        }

        inline Signature &operator[](const HMInteger &index) {
            return mSymbols[index];
        }

    };


}


#endif //HYPERMIND_SYMBOL_H
