//
// Created by 曹顺 on 2019/4/1.
//

#ifndef HYPERMIND_SYMBOL_H
#define HYPERMIND_SYMBOL_H

#include "buffer.h"
#include "lexer.h"

namespace hypermind {
    enum class SignatureType {
        None,
        Class,
        Method,
        Getter,
        Setter,
        Subscript,
        Constructor
    };

    struct Signature {
        SignatureType type{SignatureType::None};
        const HMChar *str{nullptr};
        HMUINT32 length{0};
        Signature(SignatureType type, const HMChar *str, HMUINT32 length) : type(type), str(str), length(length) {}
        Signature(const HMChar *str, HMUINT32 length) : type(SignatureType::None), str(str), length(length) {}
        Signature(const Token &token) : type(SignatureType::None), str(token.start), length(token.length) {}
        Signature(const HMChar *str) : type(SignatureType::None), str(str), length(hm_strlen(str)) {}
        explicit Signature(SignatureType type) : type(type), str(nullptr), length(0) {}
        Signature(){}

        bool operator==(const Signature&signature) {
            return type == signature.type && length == signature.length &&
            hm_memcmp(str, signature.str, length) == 0;
        }
    };

    class SymbolTable {
        Buffer<Signature> mSymbols;

        HMInteger Add(GCHeap *heap, SignatureType type, HMChar *name, HMUINT32 length) {
            mSymbols.append(heap, Signature(type, name, length));
        };

        HMInteger Add(GCHeap *heap, Signature signature) {
            mSymbols.append(heap, signature);
        };

        HMInteger Get(GCHeap *heap, Signature signature) {
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

    };
}


#endif //HYPERMIND_SYMBOL_H
