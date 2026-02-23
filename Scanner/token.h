#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    TOKEN_EOF = 0,
    TOKEN_ERROR,

    /* C keywords */
    TOKEN_INCLUDE,
    TOKEN_VOID,
    TOKEN_RETURN,
    TOKEN_INT,
    TOKEN_BOOL,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_BREAK,

    /* Arduino Sketch keywords */
    TOKEN_SERIAL,
    TOKEN_PRINT,
    TOKEN_PRINTLN,
    TOKEN_SETUP,
    TOKEN_LOOP,
    TOKEN_DELAY,
    TOKEN_PINMODE,
    TOKEN_ANALOGREAD,
    TOKEN_ANALOGWRITE,
    TOKEN_DIGITALREAD,
    TOKEN_DIGITALWRITE,

    /* Arithmetic Operators */
    TOKEN_ASSIGN,
    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_MOD,

    /* Comparison operators */
    TOKEN_EQ,
    TOKEN_NEQ,
    TOKEN_GT,
    TOKEN_GTE,
    TOKEN_LT,
    TOKEN_LTE,

    /* Boolean operators */
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,

    /* Code Formatting */
    TOKEN_SEMI,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,

    /* Other */
    TOKEN_IDENT,
    TOKEN_NUMBER,
    TOKEN_STRING

} token_t;

#endif