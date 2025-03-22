#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "../include/lexer.h"

static const char *LOOKUP_TABLE[] = {
    [TT_DT_INT] = "int",       [TT_DT_CHAR] = "char",
    [TT_DT_BOOL] = "bool",     [TT_DT_ENUM] = "enum",
    [TT_DT_FLOAT] = "float",   [TT_DT_UNION] = "union",
    [TT_DT_STRING] = "string", [TT_DT_STRUCT] = "struct",

    [TT_KW_LET] = "let",           [TT_KW_CONST] = "const",   [TT_KW_FUNC] = "func",
    [TT_KW_IF] = "if",             [TT_KW_ELSE] = "else",     [TT_KW_FOR] = "for",
    [TT_KW_WHILE] = "while",       [TT_KW_RETURN] = "return", [TT_KW_BREAK] = "break",
    [TT_KW_CONTINUE] = "continue", [TT_KW_STATIC] = "static", [TT_KW_IMPORT] = "import",

    [TT_LIT_TRUE] = "true", [TT_LIT_FALSE] = "false",

    [TT_OP_ADD] = "+",                [TT_OP_SUB] = "-",              [TT_OP_MUL] = "*",
    [TT_OP_EXP] = "**",               [TT_OP_DIV] = "/",              [TT_OP_MOD] = "%",
    [TT_OP_ASSIGN] = "=",             [TT_OP_ADD_ASSIGN] = "+=",      [TT_OP_SUB_ASSIGN] = "-=",
    [TT_OP_MUL_ASSIGN] = "*=",        [TT_OP_DIV_ASSIGN] = "/=",      [TT_OP_MOD_ASSIGN] = "%=",
    [TT_OP_INCREMENT] = "++",         [TT_OP_DECREMENT] = "--",       [TT_OP_LESS_THAN] = "<",
    [TT_OP_GREATER_THAN] = ">",       [TT_OP_LESS_THAN_EQUAL] = "<=", [TT_OP_GREATER_THAN_EQUAL] = ">=",
    [TT_OP_EQUAL] = "==",             [TT_OP_NOT_EQUAL] = "!=",       [TT_OP_AND] = "&&",
    [TT_OP_OR] = "||",                [TT_OP_NOT] = "!",              [TT_OP_BW_AND] = "&",
    [TT_OP_BW_OR] = "|",              [TT_OP_BW_XOR] = "^",           [TT_OP_BW_NOT] = "~",
    [TT_OP_BW_AND_ASSIGN] = "&=",     [TT_OP_BW_OR_ASSIGN] = "|=",    [TT_OP_BW_XOR_ASSIGN] = "^=",
    [TT_OP_BW_LSHIFT] = "<<",         [TT_OP_BW_RSHIFT] = ">>",       [TT_OP_BW_LSHIFT_ASSIGN] = "<<=",
    [TT_OP_BW_RSHIFT_ASSIGN] = ">>=", [TT_OP_ACCESS_DOT] = ".",       [TT_OP_ACCESS_ARROW] = "->",

    [TT_PUNC_COMMA] = ",",    [TT_PUNC_COLON] = ":",
    [TT_PUNC_QUESTION] = "?", [TT_PUNC_SEMICOLON] = ";",

    [TT_GRP_LPAREN] = "(", [TT_GRP_RPAREN] = ")",
    [TT_GRP_LCURLY] = "{", [TT_GRP_RCURLY] = "}",
    [TT_GRP_LBRACE] = "[", [TT_GRP_RBRACE] = "]"
};

static TokenType lookup(const char *value, TokenType start, TokenType end)
{
    for (TokenType i = start + 1; i < end; ++i)
        if (strcmp(value, LOOKUP_TABLE[i]) == 0) return i;

    return TT_ILLEGAL;
}

Token token_create(TokenType type, const char *value, size_t line, size_t column)
{
    if (type < TT_ILLEGAL || type > TT_EOF || !value || line < 1 || column < 1) {
        /// TODO: display error message
        return (Token){0};
    }

    Token token;
    token.type = type;

    strncpy(token.value, value, TOKEN_VALUE_MAX_LENGTH);
    token.value[TOKEN_VALUE_MAX_LENGTH - 1] = '\0';

    token.line = line;
    token.column = column;

    return token;
}

void token_debug(const Token *token)
{
    if (!token) {
        /// TODO: display error message
        return;
    }

    printf("Token(address=%p, type=%d, value=%s, line=%zu, column=%zu)\n",
        (void *)token, token->type, token->value, token->line, token->column);
}

TokenStream token_stream_create(void)
{
    TokenStream stream;

    stream.length = 0;
    stream.capacity = TOKEN_STREAM_INITIAL_CAPACITY;

    stream.tokens = malloc(stream.capacity * sizeof(Token));
    if (!stream.tokens) {
        /// TODO: display error message
        return (TokenStream){0};
    }

    return stream;
}

void token_stream_debug(const TokenStream *stream)
{
    if (!stream) {
        /// TODO: display error message
        return;
    }

    printf("TokenStream(address=%p, length=%zu, capacity=%zu, tokens=[\n",
        (void *)stream, stream->length, stream->capacity);

    for (size_t i = 0; i < stream->length; ++i) {
        printf("  ");
        token_debug(&stream->tokens[i]);
    }

    printf("])\n");
}

void token_stream_push(TokenStream *stream, const Token *token)
{
    if (!stream || !token) {
        /// TODO: display error message
        return;
    }

    if (stream->length >= stream->capacity) {
        stream->capacity *= TOKEN_STREAM_GROWTH_FACTOR;
        stream->tokens = realloc(stream->tokens, stream->capacity * sizeof(Token));
        if (!stream->tokens) {
            /// TODO: display error message
            return;
        }
    }

    stream->tokens[stream->length++] = *token;
}

void token_stream_destroy(TokenStream *stream)
{
    if (!stream) {
        /// TODO: display error message
        return;
    }

    free(stream->tokens);
    stream->tokens = NULL;
}

Lexer lexer_create(const char *path, const char *source)
{
    if (!path || !source) {
        /// TODO: display error message
        return (Lexer){0};
    }

    Lexer lexer;

    lexer.path = path;
    lexer.source = source;

    lexer.length = strlen(source);
    lexer.position = 0;

    lexer.line = 1;
    lexer.column = 1;

    return lexer;
}

void lexer_debug(const Lexer *lexer)
{
    if (!lexer) {
        /// TODO: display error message
        return;
    }

    printf("Lexer(address=%p, path='%s', length=%zu, \
            position=%zu, line=%zu, column=%zu)\n",
        (void *)lexer, lexer->path, lexer->length,
        lexer->position, lexer->line, lexer->column);
}

static inline int lexer_is_eof(const Lexer *lexer)
{
    return lexer->position >= lexer->length;
}

static inline char lexer_peek(const Lexer *lexer, size_t offset)
{
    return (lexer->position + offset < lexer->length)
        ? lexer->source[lexer->position + offset]
        : '\0';
}

static void lexer_advance(Lexer *lexer)
{
    switch (lexer_peek(lexer, 0)) {
        case '\n': lexer->line++; lexer->column = 1; break;
        case '\t': lexer->column += LEXER_TAB_WIDTH_INCREMENT; break;
        default: lexer->column++; break;
    }

    lexer->position++;
}

static char lexer_skip(Lexer *lexer)
{
    char current = '\0';

    while (!lexer_is_eof(lexer)) {
        current = lexer_peek(lexer, 0);
        char next = lexer_peek(lexer, 1);

        if (isspace(current)) {
            lexer_advance(lexer);
            continue;
        } else if (current == '/' && next == '/') {
            while (!lexer_is_eof(lexer) && next != '\n') {
                lexer_advance(lexer);
                next = lexer_peek(lexer, 1);
            }
            continue;
        }

        break;
    }

    return current;
}

static Token lexer_scan_identifier(Lexer *lexer, char current)
{
    size_t line = lexer->line;
    size_t column = lexer->column;

    char value[TOKEN_VALUE_MAX_LENGTH];
    size_t length = 0;

    while (!lexer_is_eof(lexer) && (isalnum(current) || current == '_')) {
        if (length >= TOKEN_VALUE_MAX_LENGTH - 1) {
            /// TODO: display error message
            return (Token){0};
        }

        value[length++] = current;
        lexer_advance(lexer);
        current = lexer_peek(lexer, 0);
    }

    value[length] = '\0';
    TokenType type = lookup(value, TT_DT_START, TT_LIT_END);
    if (type == TT_ILLEGAL) type = TT_IDENTIFIER;

    return token_create(type, value, line, column);
}

static Token lexer_scan_number(Lexer *lexer, char current)
{
    /// TODO: implement
    return (Token){0};
}

static void lexer_scan_escape(Lexer *lexer, char *buffer, size_t *length)
{
    char current = lexer_peek(lexer, 0);
    lexer_advance(lexer);

    switch (current) {
        case 'n': buffer[(*length)++] = '\n'; break;
        case 't': buffer[(*length)++] = '\t'; break;
        case 'r': buffer[(*length)++] = '\r'; break;
        case '\\': buffer[(*length)++] = '\\'; break;
        case '"': buffer[(*length)++] = '"'; break;
        case '\'': buffer[(*length)++] = '\''; break;
        case '0': buffer[(*length)++] = '\0'; break;
        default: {
            /// TODO: display error message
            buffer[(*length)++] = '\\';
            buffer[(*length)++] = current;
            break;
        }
    }
}

static Token lexer_scan_character(Lexer *lexer, char current)
{
    size_t line = lexer->line;
    size_t column = lexer->column;

    char value[TOKEN_VALUE_MAX_LENGTH];
    size_t length = 0;

    lexer_advance(lexer);
    current = lexer_peek(lexer, 0);

    if (lexer_is_eof(lexer) || current == '\'') {
        /// TODO: display error message
        return (Token){0};
    }

    if (current == '\\') {
        lexer_advance(lexer);
        lexer_scan_escape(lexer, value, &length);
    } else {
        value[length++] = current;
        lexer_advance(lexer);
    }

    current = lexer_peek(lexer, 0);
    if (current != '\'') {
        /// TODO: display error message
        return (Token){0};
    }

    lexer_advance(lexer);
    value[length] = '\0';

    if (length > 1) {
        /// TODO: display error message
        return (Token){0};
    }

    return token_create(TT_LIT_CHAR, value, line, column);
}

static Token lexer_scan_string(Lexer *lexer, char current)
{
    size_t line = lexer->line;
    size_t column = lexer->column;

    char value[TOKEN_VALUE_MAX_LENGTH];
    size_t length = 0;

    lexer_advance(lexer);
    current = lexer_peek(lexer, 0);

    while (!lexer_is_eof(lexer) && current != '"') {
        if (length >= TOKEN_VALUE_MAX_LENGTH - 1) {
            /// TODO: display error message
            return (Token){0};
        }

        if (current == '\\') {
            lexer_advance(lexer);
            lexer_scan_escape(lexer, value, &length);
        } else {
            value[length++] = current;
        }

        lexer_advance(lexer);
        current = lexer_peek(lexer, 0);
    }

    if (current != '"') {
        /// TODO: display error message
        return (Token){0};
    }

    lexer_advance(lexer);
    value[length] = '\0';

    return token_create(TT_LIT_STRING, value, line, column);
}

static Token lexer_scan_operator(Lexer *lexer, char current)
{
    size_t line = lexer->line;
    size_t column = lexer->column;

    char value[TOKEN_VALUE_MAX_LENGTH];
    size_t length = 0;

    while (!lexer_is_eof(lexer) && strchr("+-*/%=<>!&|^~.", current)) {
        if (length >= TOKEN_VALUE_MAX_LENGTH - 1) {
            /// TODO: display error message
            return (Token){0};
        }

        value[length++] = current;
        lexer_advance(lexer);
        current = lexer_peek(lexer, 0);
    }

    value[length] = '\0';
    TokenType type = lookup(value, TT_OP_START, TT_OP_END);
    if (type == TT_ILLEGAL) {
        /// TODO: display error message
    }

    return token_create(type, value, line, column);
}

static Token lexer_scan_special(Lexer *lexer, char current)
{
    char value[2] = {current, '\0'};
    lexer_advance(lexer);

    TokenType type = lookup(value, TT_PUNC_START, TT_GRP_END);
    if (type == TT_ILLEGAL) {
        /// TODO: display error message
    }

    return token_create(type, value, lexer->line, lexer->column - 1);
}

Token lexer_next(Lexer *lexer)
{
    if (!lexer) {
        /// TODO: display error message
        return (Token){0};
    }

    char current = lexer_skip(lexer);
    if (lexer_is_eof(lexer))
        return token_create(TT_EOF, "EOF", lexer->line, lexer->column);

    if (isalpha(current) || current == '_')
        return lexer_scan_identifier(lexer, current);
    else if (isdigit(current))
        return lexer_scan_number(lexer, current);
    else if (current == '\'')
        return lexer_scan_character(lexer, current);
    else if (current == '"')
        return lexer_scan_string(lexer, current);
    else if (strchr("+-*/%=<>!&|^~.", current))
        return lexer_scan_operator(lexer, current);

    return lexer_scan_special(lexer, current);
}

TokenStream lexer_scan(Lexer *lexer)
{
    if (!lexer) {
        /// TODO: display error message
        return (TokenStream){0};
    }

    TokenStream stream = token_stream_create();
    if (!stream.tokens) return (TokenStream){0};

    Token token;
    do {
        token = lexer_next(lexer);
        if (token.type == TT_ERROR) {
            /// TODO: display error message
            token_stream_destroy(&stream);
            return (TokenStream){0};
        }
        token_stream_push(&stream, &token);
    } while (token.type != TT_ILLEGAL && token.type != TT_EOF);

    return stream;
}