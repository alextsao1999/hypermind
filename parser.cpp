//
// Created by 曹顺 on 2019/2/11.
//

#include "parser.h"

namespace hypermind{

    Parser::Parser(const char *mFileName, char *mSource, const Lexer &mLexer) : mFileName(mFileName), mSource(mSource),
                                                                                mLexer(mLexer) {}
}