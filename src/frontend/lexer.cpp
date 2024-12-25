#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../include/frontend/lexer.hpp"

namespace Jewel {

    static const std::unordered_map<std::string, TokenType> LOOKUP_TABLE = {
        {"let", KW_LET},       {"func", KW_FUNC},     {"if", KW_IF},           {"else", KW_ELSE},
        {"for", KW_FOR},       {"while", KW_WHILE},   {"return", KW_RETURN},   {"break", KW_BREAK},
        {"switch", KW_SWITCH}, {"case", KW_CASE},     {"default", KW_DEFAULT}, {"continue", KW_CONTINUE},
        {"static", KW_STATIC}, {"const", KW_CONST},   {"null", KW_NULL},       {"int", KW_INT},
        {"char", KW_CHAR},     {"bool", KW_BOOL},     {"enum", KW_ENUM},       {"float", KW_FLOAT},
        {"union", KW_UNION},   {"string", KW_STRING}, {"struct", KW_STRUCT},   {"import", KW_IMPORT},

        {"true", LIT_TRUE}, {"false", LIT_FALSE},

        {"+", OP_PLUS},      {"-", OP_MINUS},  {"*", OP_MULTIPLY},
        {"**", OP_EXPONENT}, {"/", OP_DIVIDE}, {"%", OP_MODULO},

        {"=", OP_ASSIGN},           {"+=", OP_PLUS_ASSIGN},   {"-=", OP_MINUS_ASSIGN},
        {"*=", OP_MULTIPLY_ASSIGN}, {"/=", OP_DIVIDE_ASSIGN}, {"%=", OP_MODULO_ASSIGN},

        {"++", OP_INCREMENT}, {"--", OP_DECREMENT},

        {"<", OP_LESS_THAN},           {">", OP_GREATER_THAN}, {"<=", OP_LESS_THAN_EQUAL},
        {">=", OP_GREATER_THAN_EQUAL}, {"==", OP_EQUAL},       {"!=", OP_NOT_EQUAL},

        {"&&", OP_LG_AND}, {"||", OP_LG_OR}, {"!", OP_LG_NOT},

        {"&", OP_BW_AND}, {"|", OP_BW_OR},
        {"^", OP_BW_XOR}, {"~", OP_BW_NOT},

        {"&=", OP_BW_AND_ASSIGN}, {"|=", OP_BW_OR_ASSIGN}, {"^=", OP_BW_XOR_ASSIGN},

        {"<<", OP_BS_LEFT}, {">>", OP_BS_RIGHT},

        {".", PUNC_DOT},       {",", PUNC_COMMA},    {":", PUNC_COLON},
        {";", PUNC_SEMICOLON}, {"?", PUNC_QUESTION}, {"#", PUNC_HASH},

        {"(", GRP_LPAREN}, {")", GRP_RPAREN}, {"{", GRP_LCURLY},
        {"}", GRP_RCURLY}, {"[", GRP_LBRACE}, {"]", GRP_RBRACE}
    };

    static bool IsOperator(char current)
    {
        return current == '+' || current == '-' || current == '*' ||
            current == '/' || current == '=' || current == '>' || current == '<' ||
            current == '!' || current == '&' || current == '|' || current == '^' ||
            current == '~' || current == '%';
    }
    
    Token::Token(TokenType type, const std::string& lexeme, unsigned int line, unsigned int column)
        : type(type), lexeme(lexeme), line(line), column(column)
    {}

    void Token::Debug(void) const
    {
        std::cout << "Token { address: " << this << ", type: " << type << ", lexeme: "
            << lexeme << ", line: " << line << ", column: " << column << " }\n";
    }

    Lexer::Lexer(const std::string& path, const std::string& source)
        : path(path), source(source), length(source.length()),
          position(0), line(1), column(1)
    {}
    
    void Lexer::Debug(void) const
    {
        std::cout << "Lexer { address: " << this << ", path: " << path << ", length: " << length <<
            ", position: " << position << ", line: " << line << ", column: " << column << " }\n";
    }
    
    std::vector<Token> Lexer::Tokenize(void)
    {
        std::vector<Token> tokens;
        while (true) {
            Token token = Next();
            tokens.push_back(token);
            if (token.type == END_OF_FILE)
                break;
        }
        return tokens;
    }

    void Lexer::Advance(void)
    {
        if (position < length) {
            char current =  source[position];
            if (current == '\n') {
                line++;
                column = 0;
            } else if (current == '\t') {
                column += 3;
            }
            column++;
            position++;
        }
    }
    
    void Lexer::Skip(void)
    {
        while (position < length) {
            char current = source[position];
            if (std::isspace(current)) {
                Advance();
            } else if (current == '/') {
                if (position + 1 < length && source[position + 1] == '/') {
                    while (position < length && source[position] != '\n')
                        Advance();
                } else {
                    break;
                }
            } else {
                break;
            }
        }
    }
    
    Token Lexer::Next(void)
    {
        Skip();
        if (position >= length)
            return Token(END_OF_FILE, "\0", line, column);
        char current = source[position];
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
        return Special(current);
    }

    Token Lexer::Alphanumeric(void)
    {
        unsigned int startPos = position;
        unsigned int startCol = column;
        while (position < length && (std::isalnum(source[position]) || source[position] == '_'))
            Advance();
        std::string lexeme = source.substr(startPos, position - startPos);
        auto it = LOOKUP_TABLE.find(lexeme);
        if (it != LOOKUP_TABLE.end())
            return Token(it->second, lexeme, line, startCol);
        return Token(IDENTIFIER, lexeme, line, startCol);
    }
    
    Token Lexer::Numeric(void)
    {
        // TODO: implement
        return Token(ILLEGAL, "", line, column);
    }
    
    Token Lexer::Character(void)
    {
        // TODO: implement
        return Token(ILLEGAL, "", line, column);
    }
    
    Token Lexer::String(void)
    {
        // TODO: implement
        return Token(ILLEGAL, "", line, column);
    }
    
    Token Lexer::Operator(void)
    {
        unsigned int startPos = position;
        unsigned int startCol = column;
        while (position < length && IsOperator(source[position]))
            Advance();
        std::string lexeme = source.substr(startPos, position - startPos);
        auto it = LOOKUP_TABLE.find(lexeme);
        if (it != LOOKUP_TABLE.end())
            return Token(it->second, lexeme, line, startCol);
        return Token(ILLEGAL, lexeme, line, startCol);
    }
    
    Token Lexer::Special(char current)
    {
        Advance();
        std::string lexeme = std::string(1, current);
        auto it = LOOKUP_TABLE.find(lexeme);
        if (it != LOOKUP_TABLE.end())
            return Token(it->second, lexeme, line, column - 1);
        return Token(ILLEGAL, lexeme, line, column - 1);
    }

}