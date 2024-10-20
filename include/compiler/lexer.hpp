#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace Compiler {

    struct Token
    {
        enum class Type { ILLEGAL, KEYWORD, LITERAL, OPERATOR, IDENTIFIER, PUNCTUATION, GROUPING, END_OF_FILE };

        Type Type;
        std::string Lexeme;
        unsigned int Line;
        unsigned int Column;

        Token(Type type, const std::string& lexeme, unsigned int line, unsigned int column);
        void Debug(void) const;
    };

    class Lexer
    {
    public:
        Lexer(const std::string& path, const std::string& source);
        std::unique_ptr<Token> NextToken(void);
        std::vector<std::unique_ptr<Token>> Tokenize(void);
        void Debug(void) const;

    private:
        std::string m_Path;
        std::string m_Source;
        unsigned int m_SourceLen;
        unsigned int m_Position;
        unsigned int m_Line;
        unsigned int m_Column;

    private:
        void Advance(void);
        void HandleWhitespaceAndComments(void);
    };

}