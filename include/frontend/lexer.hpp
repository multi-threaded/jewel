#pragma once

#include <string>
#include <vector>

namespace Jewel {

    enum TokenType
    {
        ILLEGAL,

        KW_LET,    KW_FUNC,   KW_IF,      KW_ELSE,
        KW_FOR,    KW_WHILE,  KW_RETURN,  KW_BREAK,
        KW_SWITCH, KW_CASE,   KW_DEFAULT, KW_CONTINUE,
        KW_STATIC, KW_CONST,  KW_NULL,    KW_INT,
        KW_CHAR,   KW_BOOL,   KW_ENUM,    KW_FLOAT,
        KW_UNION,  KW_STRING, KW_STRUCT,  KW_IMPORT,

        LIT_INT,   LIT_CHAR,  LIT_TRUE,
        LIT_FALSE, LIT_FLOAT, LIT_STRING,

        OP_PLUS,     OP_MINUS,  OP_MULTIPLY,
        OP_EXPONENT, OP_DIVIDE, OP_MODULO,

        OP_ASSIGN,          OP_PLUS_ASSIGN,   OP_MINUS_ASSIGN,
        OP_MULTIPLY_ASSIGN, OP_DIVIDE_ASSIGN, OP_MODULO_ASSIGN,

        OP_INCREMENT, OP_DECREMENT,

        OP_LESS_THAN,          OP_GREATER_THAN, OP_LESS_THAN_EQUAL,
        OP_GREATER_THAN_EQUAL, OP_EQUAL,        OP_NOT_EQUAL,

        OP_LG_AND, OP_LG_OR, OP_LG_NOT,

        OP_BW_AND, OP_BW_OR,
        OP_BW_XOR, OP_BW_NOT,

        OP_BW_AND_ASSIGN, OP_BW_OR_ASSIGN, OP_BW_XOR_ASSIGN,

        OP_BS_LEFT, OP_BS_RIGHT,

        IDENTIFIER,

        PUNC_DOT,       PUNC_COMMA,    PUNC_COLON,
        PUNC_SEMICOLON, PUNC_QUESTION, PUNC_HASH,

        GRP_LPAREN, GRP_RPAREN, GRP_LCURLY,
        GRP_RCURLY, GRP_LBRACE, GRP_RBRACE,

        END_OF_FILE
    };

    struct Token
    {
        TokenType type;
        std::string lexeme;
        unsigned int line;
        unsigned int column;

        Token(TokenType type, const std::string& lexeme, unsigned int line, unsigned int column);
        void Debug(void) const;
    };

    class Lexer
    {
    public:
        Lexer(const std::string& path, const std::string& source);
        void Debug(void) const;
        std::vector<Token> Tokenize(void);

    private:
        std::string path;
        std::string source;
        unsigned int length;
        unsigned int position;
        unsigned int line;
        unsigned int column;

    private:
        void Advance(void);
        void Skip(void);
        Token Next(void);
        Token Alphanumeric(void);
        Token Numeric(void);
        Token Character(void);
        Token String(void);
        Token Operator(void);
        Token Special(char current);
    };

}