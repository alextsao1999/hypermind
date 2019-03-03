//
// Created by 曹顺 on 2019/2/11.
//

#include "parser.h"

namespace hypermind{
    SymbolBindRule::SymbolBindRule(const HMChar *id, DenotationFn nud, DenotationFn led,
                                   MethodSignatureFn methodSign): id(id), nud(nud), led(led),
                                                                  methodSign(methodSign) {
    }

    SymbolBindRule Rules[] = {
            SymbolBindRule("-", (DenotationFn) &Parser::BindFunction, (DenotationFn) &Parser::BindFunction,
                           (MethodSignatureFn) &Parser::Signature),
            SymbolBindRule("-", (DenotationFn) &Parser::BindFunction, (DenotationFn) &Parser::BindFunction,
                           (MethodSignatureFn) &Parser::Signature),

    };

}