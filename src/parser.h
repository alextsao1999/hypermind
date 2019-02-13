//
// Created by 曹顺 on 2019/2/11.
//

#ifndef HYPERMIND_PARSER_H
#define HYPERMIND_PARSER_H

#include <lexer.h>
#include <hash_map>
namespace hypermind{
    class Parser {
    protected:
        const char *mFileName;
        Lexer mLexer;

    public:
        Parser(const char *mFileName, const Lexer &mLexer);



    };

}


#endif //HYPERMIND_PARSER_H
