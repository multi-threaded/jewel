#pragma once

#include <string>
#include <memory>
#include <vector>

namespace Compiler {

    enum class TokenType
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

    const std::string& TokenTypeToString(TokenType type);

    struct Token
    {
        TokenType Type;
        std::string Lexeme;
        unsigned int Line;
        unsigned int Column;

        Token(TokenType type, const std::string& lexeme, unsigned int line, unsigned int column);
        void Debug() const;
    };

    class Lexer
    {
    public:
        Lexer(const std::string& path, const std::string& source);
        std::unique_ptr<Token> NextToken();
        std::vector<std::unique_ptr<Token>> Tokenize();
        void Debug() const;

    private:
        std::string m_Path;
        std::string m_Source;
        unsigned int m_SourceLen;
        unsigned int m_Position;
        unsigned int m_Line;
        unsigned int m_Column;

    private:
        void Advance();
    };

}