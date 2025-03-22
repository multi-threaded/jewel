#ifndef JEWEL_LEXER_H
#define JEWEL_LEXER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#define TOKEN_VALUE_MAX_LENGTH 1024
#define TOKEN_STREAM_INITIAL_CAPACITY 10
#define TOKEN_STREAM_GROWTH_FACTOR 2
#define LEXER_TAB_WIDTH_INCREMENT 4

typedef enum {
    TT_ERROR, TT_ILLEGAL,

    TT_DT_START,
    TT_DT_INT,    TT_DT_CHAR,
    TT_DT_BOOL,   TT_DT_ENUM,
    TT_DT_FLOAT,  TT_DT_UNION,
    TT_DT_STRING, TT_DT_STRUCT,
    TT_DT_END,

    TT_KW_START,
    TT_KW_LET,      TT_KW_CONST,  TT_KW_FUNC,
    TT_KW_IF,       TT_KW_ELSE,   TT_KW_FOR,
    TT_KW_WHILE,    TT_KW_RETURN, TT_KW_BREAK,
    TT_KW_CONTINUE, TT_KW_STATIC, TT_KW_IMPORT,
    TT_KW_END,

    TT_LIT_START,
    TT_LIT_INT,   TT_LIT_CHAR,
    TT_LIT_TRUE,  TT_LIT_FALSE,
    TT_LIT_FLOAT, TT_LIT_STRING,
    TT_LIT_END,

    TT_OP_START,
    TT_OP_ADD,                TT_OP_SUB,             TT_OP_MUL,
    TT_OP_EXP,                TT_OP_DIV,             TT_OP_MOD,
    TT_OP_ASSIGN,             TT_OP_ADD_ASSIGN,      TT_OP_SUB_ASSIGN,
    TT_OP_MUL_ASSIGN,         TT_OP_DIV_ASSIGN,      TT_OP_MOD_ASSIGN,
    TT_OP_INCREMENT,          TT_OP_DECREMENT,       TT_OP_LESS_THAN,
    TT_OP_GREATER_THAN,       TT_OP_LESS_THAN_EQUAL, TT_OP_GREATER_THAN_EQUAL,
    TT_OP_EQUAL,              TT_OP_NOT_EQUAL,       TT_OP_AND,
    TT_OP_OR,                 TT_OP_NOT,             TT_OP_BW_AND,
    TT_OP_BW_OR,              TT_OP_BW_XOR,          TT_OP_BW_NOT,
    TT_OP_BW_AND_ASSIGN,      TT_OP_BW_OR_ASSIGN,    TT_OP_BW_XOR_ASSIGN,
    TT_OP_BW_LSHIFT,          TT_OP_BW_RSHIFT,       TT_OP_BW_LSHIFT_ASSIGN,
    TT_OP_BW_RSHIFT_ASSIGN,   TT_OP_ACCESS_DOT,      TT_OP_ACCESS_ARROW,
    TT_OP_END,

    TT_IDENTIFIER,

    TT_PUNC_START,
    TT_PUNC_COMMA,    TT_PUNC_COLON,
    TT_PUNC_QUESTION, TT_PUNC_SEMICOLON,
    TT_PUNC_END,

    TT_GRP_START,
    TT_GRP_LPAREN, TT_GRP_RPAREN,
    TT_GRP_LCURLY, TT_GRP_RCURLY,
    TT_GRP_LBRACE, TT_GRP_RBRACE,
    TT_GRP_END,

    TT_EOF
} TokenType;

typedef struct {
    TokenType type;
    char value[TOKEN_VALUE_MAX_LENGTH];
    size_t line;
    size_t column;
} Token;

Token token_create(TokenType type, const char *value, size_t line, size_t column);
void token_debug(const Token *token);

typedef struct {
    Token *tokens;
    size_t length;
    size_t capacity;
} TokenStream;

TokenStream token_stream_create(void);
void token_stream_debug(const TokenStream *stream);
void token_stream_push(TokenStream *stream, const Token *token);
void token_stream_destroy(TokenStream *stream);

typedef struct {
    const char *path;
    const char *source;
    size_t length;
    size_t position;
    size_t line;
    size_t column;
} Lexer;

Lexer lexer_create(const char *path, const char *source);
void lexer_debug(const Lexer *lexer);
Token lexer_next(Lexer *lexer);
TokenStream lexer_scan(Lexer *lexer);

#ifdef __cplusplus
}
#endif

#endif // !JEWEL_LEXER_H