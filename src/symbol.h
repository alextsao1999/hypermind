//
// Created by 曹顺 on 2019/4/1.
//

#ifndef HYPERMIND_SYMBOL_H
#define HYPERMIND_SYMBOL_H

#include "buffer.h"

namespace hypermind {
    enum class SignatureType {
        None,
        Method,
        Getter,
        Setter,
        Subscript,
        Constructor
    };

    struct Signature {
        SignatureType type;
        const HMChar *str;
        HMUINT32 length;
        Signature(SignatureType type, const HMChar *str, HMUINT32 length) : type(type), str(str), length(length) {}
        explicit Signature(SignatureType type) : type(type), str(nullptr), length(0) {}
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
                if (mSymbols[i].type == signature.type && mSymbols[i].length == signature.length &&
                    hm_memcmp(mSymbols[i].str, signature.str, signature.length) == 0) {
                    return i;
                }
            }
            return -1;
        };

    };
}


#endif //HYPERMIND_SYMBOL_H
