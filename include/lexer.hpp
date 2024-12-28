#pragma once

#include <string>
#include <vector>

namespace Jewel {

    enum TokenType
    {
        ILLEGAL,
        KEYWORD,
        LITERAL,
        OPERATOR,
        IDENTIFIER,
        PUNCTUATION,
        GROUPING,
        END_OF_FILE
    };

    struct Token
    {
        TokenType Type;
        std::string Lexeme;
        size_t Line;
        size_t Column;
    };

    class Lexer
    {
    public:
        Lexer(const std::string& path, const std::string& source);
        Token Next(void);
        std::vector<Token> Tokenize(void);

    private:
        std::string m_Path;
        std::string m_Source;
        size_t m_SourceLen;
        size_t m_Position;
        size_t m_Line;
        size_t m_Column;

    private:
        void Advance(void);
        void Skip(void);
        Token Alphanumeric(void);
        Token Numeric(void);
        Token Character(void);
        Token String(void);
        Token Operator(void);
        Token Special(void);
    };

}