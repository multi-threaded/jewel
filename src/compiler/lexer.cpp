#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include "../../include/compiler/lexer.hpp"

namespace Compiler {

    const std::string& TokenTypeToString(TokenType type)
    {
        switch (type) {
            case TokenType::KEYWORD: return "KEYWORD";
            case TokenType::LITERAL: return "LITERAL";
            case TokenType::OPERATOR: return "OPERATOR";
            case TokenType::IDENTIFIER: return "IDENTIFIER";
            case TokenType::PUNCTUATION: return "PUNCTUATION";
            case TokenType::GROUPING: return "GROUPING";
            case TokenType::END_OF_FILE: return "END_OF_FILE";
            default: return "ILLEGAL";
        }
    }

    Token::Token(TokenType type, const std::string& lexeme, unsigned int line, unsigned int column)
        : Type(type), Lexeme(lexeme), Line(line), Column(column)
    {}

    void Token::Debug() const
    {
        std::cout << "Token(Type=" << TokenTypeToString(Type) << ", Lexeme=" <<
            Lexeme << ", Line=" << Line << ", Column=" << Column << ")\n";
    }

    Lexer::Lexer(const std::string& path, const std::string& source)
        : m_Path(path), m_Source(source), m_SourceLen(source.length()),
          m_Position(0), m_Line(1), m_Column(1)
    {}
    
    std::unique_ptr<Token> Lexer::NextToken()
    {
        // TODO: implement
        return nullptr;
    }
    
    std::vector<std::unique_ptr<Token>> Lexer::Tokenize()
    {
        std::vector<std::unique_ptr<Token>> tokens;
        std::unique_ptr<Token> token;

        while ((token = NextToken()) != nullptr) {
            tokens.push_back(std::move(token));
            if (tokens.back()->Type == TokenType::END_OF_FILE)
                break;
        }

        return tokens;
    }

    void Lexer::Debug() const
    {
        std::cout << "Lexer(Path=\"" << m_Path << "\", SourceLen="
            << m_SourceLen << ", Position=" << m_Position << ", Line="
            << m_Line << ", Column=" << m_Column << ")\n";
    }

    void Lexer::Advance()
    {
        if (m_Position >= m_SourceLen)
            return;

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