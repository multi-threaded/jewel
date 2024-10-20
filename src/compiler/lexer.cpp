#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include "../../include/compiler/lexer.hpp"

namespace Compiler {

    Token::Token(Token::Type type, const std::string& lexeme, unsigned int line, unsigned int column)
        : m_Type(type), m_Lexeme(lexeme), m_Line(line), m_Column(column)
    {}

    void Token::Debug(void) const
    {
        std::cout << "Token(Type=" << static_cast<int>(m_Type) << ", Lexeme=" <<
            m_Lexeme << ", Line=" << m_Line << ", Column=" << m_Column << ")\n";
    }

    static const std::unordered_map<std::string, Token::Type> LOOKUP_TABLE = {
        // TODO: implement
    };

    Lexer::Lexer(const std::string& path, const std::string& source)
        : m_Path(path), m_Source(source), m_SourceLen(source.length()),
          m_Position(0), m_Line(1), m_Column(1)
    {}
    
    std::unique_ptr<Token> Lexer::NextToken(void)
    {
        HandleWhitespaceAndComments();

        if (m_Position >= m_SourceLen)
            return std::make_unique<Token>(Token::Type::END_OF_FILE, "\0", m_Line, m_Column);

        // TODO: finish implementing

        return nullptr;
    }
    
    std::vector<std::unique_ptr<Token>> Lexer::Tokenize(void)
    {
        std::vector<std::unique_ptr<Token>> tokens;
        std::unique_ptr<Token> token;

        while ((token = NextToken()) != nullptr) {
            tokens.push_back(std::move(token));
            if (tokens.back()->m_Type == Token::Type::END_OF_FILE)
                break;
        }

        return tokens;
    }

    void Lexer::Debug(void) const
    {
        std::cout << "Lexer(Path=\"" << m_Path << "\", SourceLen="
            << m_SourceLen << ", Position=" << m_Position << ", Line="
            << m_Line << ", Column=" << m_Column << ")\n";
    }

    void Lexer::Advance(void)
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

    void Lexer::HandleWhitespaceAndComments(void)
    {
        while (m_Position < m_SourceLen) {
            char current = m_Source[m_Position];
            if (isspace(current)) {
                Advance();
                continue;
            }

            if (current == '/' && m_Position + 1 < m_SourceLen && m_Source[m_Position + 1] == '/') {
                while (current != '\n' && m_Position < m_SourceLen) {
                    Advance();
                    current = m_Source[m_Position];
                }
                continue;
            }

            break;
        }
    }

}