#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../include/lexer.hpp"

namespace Jewel {

    static const std::unordered_map<std::string, TokenType> LOOKUP_TABLE = {
        {"let", KEYWORD},    {"func", KEYWORD},   {"if", KEYWORD},      {"else", KEYWORD},
        {"for", KEYWORD},    {"while", KEYWORD},  {"return", KEYWORD},  {"break", KEYWORD},
        {"switch", KEYWORD}, {"case", KEYWORD},   {"default", KEYWORD}, {"continue", KEYWORD},
        {"static", KEYWORD}, {"const", KEYWORD},  {"null", KEYWORD},    {"int", KEYWORD},
        {"char", KEYWORD},   {"bool", KEYWORD},   {"enum", KEYWORD},    {"float", KEYWORD},
        {"union", KEYWORD},  {"string", KEYWORD}, {"struct", KEYWORD},  {"import", KEYWORD},

        {"true", LITERAL}, {"false", LITERAL},

        {"+", OPERATOR},  {"-", OPERATOR},  {"*", OPERATOR},  {"**", OPERATOR},
        {"/", OPERATOR},  {"%", OPERATOR},  {"=", OPERATOR},  {"+=", OPERATOR},
        {"-=", OPERATOR}, {"*=", OPERATOR}, {"/=", OPERATOR}, {"%=", OPERATOR},
        {"++", OPERATOR}, {"--", OPERATOR}, {"<", OPERATOR},  {">", OPERATOR},
        {"<=", OPERATOR}, {">=", OPERATOR}, {"==", OPERATOR}, {"!=", OPERATOR},
        {"&&", OPERATOR}, {"||", OPERATOR}, {"!", OPERATOR},  {"&", OPERATOR},
        {"|", OPERATOR},  {"^", OPERATOR},  {"~", OPERATOR},  {"&=", OPERATOR},
        {"|=", OPERATOR}, {"^=", OPERATOR}, {"<<", OPERATOR}, {">>", OPERATOR},

        {".", PUNCTUATION}, {",", PUNCTUATION}, {":", PUNCTUATION},
        {";", PUNCTUATION}, {"?", PUNCTUATION}, {"#", PUNCTUATION},

        {"(", GROUPING}, {")", GROUPING}, {"{", GROUPING},
        {"}", GROUPING}, {"[", GROUPING}, {"]", GROUPING}
    };

    static bool IsOperator(char current)
    {
        return current == '+' || current == '-' || current == '*' || current == '/' ||
               current == '=' || current == '>' || current == '<' || current == '!' ||
               current == '&' || current == '|' || current == '^' || current == '~' ||
               current == '%';
    }

    Lexer::Lexer(const std::string& path, const std::string& source)
        : m_Path(path), m_Source(source), m_SourceLen(source.length()),
          m_Position(0), m_Line(1), m_Column(1)
    {}

    Token Lexer::Next(void)
    {
        Skip();
        if (m_Position >= m_SourceLen)
            return Token{END_OF_FILE, "\0", m_Line, m_Column};

        char current = m_Source[m_Position];
        if (std::isalpha(current) || current == '_')
            return Alphanumeric();
        else if (std::isdigit(current))
            return Numeric();
        else if (current == '\'')
            return Character();
        else if (current == '"')
            return String();
        else if (IsOperator(current))
            return Operator();
        return Special();
    }

    std::vector<Token> Lexer::Tokenize(void)
    {
        std::vector<Token> tokens;

        while (true) {
            Token token = Next();
            tokens.push_back(token);
            if (token.Type == ILLEGAL || token.Type == END_OF_FILE)
                break;
        }

        return tokens;
    }

    void Lexer::Advance(void)
    {
        if (m_Position < m_SourceLen) {
            char current = m_Source[m_Position];
            if (current == '\n') {
                m_Line++;
                m_Column = 0;
            } else if (current == '\t') {
                m_Column += 3;
            }
            m_Column++;
            m_Position++;
        }
    }
    
    void Lexer::Skip(void)
    {
        while (m_Position < m_SourceLen) {
            char current = m_Source[m_Position];
            if (std::isspace(current)) {
                Advance();
                continue;
            } else if (current == '/' && m_Position + 1 < m_SourceLen && m_Source[m_Position + 1] == '/') {
                while (m_Position < m_SourceLen && current != '\n') {
                    Advance();
                    current = m_Source[m_Position];
                }
                continue;
            }
            break;
        }
    }
    
    Token Lexer::Alphanumeric(void)
    {
        size_t startCol = m_Column;
        size_t startPos = m_Position;
        while (m_Position < m_SourceLen && (std::isalnum(m_Source[m_Position]) || m_Source[m_Position] == '_'))
            Advance();

        std::string lexeme = m_Source.substr(startPos, m_Position - startPos);
        auto it = LOOKUP_TABLE.find(lexeme);
        if (it != LOOKUP_TABLE.end())
            return Token{it->second, lexeme, m_Line, startCol};

        return Token{IDENTIFIER, lexeme, m_Line, startCol};
    }
    
    Token Lexer::Numeric(void)
    {
        size_t startCol = m_Column;
        size_t startPos = m_Position;
        size_t decimalCount = 0;

        while (m_Position < m_SourceLen && (std::isdigit(m_Source[m_Position]) || m_Source[m_Position] == '.')) {
            if (m_Source[m_Position] == '.')
                decimalCount++;
            Advance();
        }

        std::string lexeme = m_Source.substr(startPos, m_Position - startPos);
        if (decimalCount > 1) {
            std::cerr << "SyntaxError: invalid numeric literal \"" << lexeme << "\" in '"
                << m_Path << "' at line " << m_Line << ", column " << startCol << "\n";
            return Token{ILLEGAL, lexeme, m_Line, startCol};
        }

        return Token{LITERAL, lexeme, m_Line, startCol};
    }
    
    Token Lexer::Character(void)
    {
        size_t startCol = m_Column;
        size_t startPos = m_Position;
        std::string lexeme;

        Advance();
        if (m_Position >= m_SourceLen || m_Source[m_Position] == '\'') {
            lexeme = m_Source.substr(startPos, m_Position - startPos);
            std::cerr << "SyntaxError: empty character literal in '" << m_Path
                << "' at line " << m_Line << ", column " << startCol << "\n";
            return Token{ILLEGAL, lexeme, m_Line, startCol};
        }

        Advance();
        if (m_Position >= m_SourceLen || m_Source[m_Position] != '\'') {
            lexeme = m_Source.substr(startPos, m_Position - startPos);
            std::cerr << "SyntaxError: unterminated character \"" << lexeme << "\" literal in '"
                << m_Path << "' at line " << m_Line << ", column " << startCol << "\n";
            return Token{ILLEGAL, lexeme, m_Line, startCol};
        }

        Advance();
        lexeme = m_Source.substr(startPos, m_Position - startPos);

        return Token{LITERAL, lexeme, m_Line, startCol};
    }
    
    Token Lexer::String(void)
    {
        // TODO: implement string escape sequences
        size_t startCol = m_Column;
        size_t startPos = m_Position;
        std::string lexeme;

        Advance();
        while (m_Position < m_SourceLen && m_Source[m_Position] != '"')
            Advance();

        if (m_Position >= m_SourceLen || m_Source[m_Position] != '"') {
            lexeme = m_Source.substr(startPos, m_Position - startPos);
            std::cerr << "SyntaxError: unterminated string literal \"" << lexeme << "\" in '"
                << m_Path << "' at line " << m_Line << ", column " << m_Column << "\n";
            return Token{ILLEGAL, lexeme, m_Line, startCol};
        }

        Advance();
        lexeme = m_Source.substr(startPos, m_Position - startPos);

        return Token{LITERAL, lexeme, m_Line, startCol};
    }
    
    Token Lexer::Operator(void)
    {
        size_t startCol = m_Column;
        size_t startPos = m_Position;
        while (m_Position < m_SourceLen && IsOperator(m_Source[m_Position]))
            Advance();

        std::string lexeme = m_Source.substr(startPos, m_Position - startPos);
        auto it = LOOKUP_TABLE.find(lexeme);
        if (it != LOOKUP_TABLE.end())
            return Token{it->second, lexeme, m_Line, startCol};

        std::cerr << "SyntaxError: invalid operator \"" << lexeme << "\" in '" <<
            m_Path << "' at line " << m_Line << ", column " << m_Column << "\n";
        return Token{ILLEGAL, lexeme, m_Line, startCol};
    }
    
    Token Lexer::Special(void)
    {
        std::string lexeme(1, m_Source[m_Position]);
        Advance();

        auto it = LOOKUP_TABLE.find(lexeme);
        if (it != LOOKUP_TABLE.end())
            return Token{it->second, lexeme, m_Line, m_Column - 1};

        std::cerr << "SyntaxError: invalid token \"" << lexeme << "\" in '" <<
            m_Path << "' at line " << m_Line << ", column " << m_Column - 1 << "\n";
        return Token{ILLEGAL, lexeme, m_Line, m_Column - 1};
    }

}