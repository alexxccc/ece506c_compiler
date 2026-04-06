%{
#include <stdio.h>
#include <stdlib.h>

extern int yylex(void);
extern FILE *yyin;
void yyerror(const char *s);
%}

/* Keywords */
%token TOKEN_ERROR
%token TOKEN_INCLUDE
%token TOKEN_VOID
%token TOKEN_RETURN
%token TOKEN_INT
%token TOKEN_BOOL
%token TOKEN_IF
%token TOKEN_ELSE
%token TOKEN_WHILE
%token TOKEN_TRUE
%token TOKEN_FALSE
%token TOKEN_BREAK

/* Arduino-specific */
%token TOKEN_SERIAL
%token TOKEN_PRINT
%token TOKEN_PRINTLN
%token TOKEN_SETUP
%token TOKEN_LOOP
%token TOKEN_DELAY
%token TOKEN_PINMODE
%token TOKEN_ANALOGREAD
%token TOKEN_ANALOGWRITE
%token TOKEN_DIGITALREAD
%token TOKEN_DIGITALWRITE

/* Operators */
%token TOKEN_ASSIGN
%token TOKEN_ADD
%token TOKEN_SUB
%token TOKEN_MUL
%token TOKEN_DIV
%token TOKEN_MOD

%token TOKEN_EQ
%token TOKEN_NEQ
%token TOKEN_GT
%token TOKEN_GTE
%token TOKEN_LT
%token TOKEN_LTE

%token TOKEN_AND
%token TOKEN_OR
%token TOKEN_NOT

/* Punctuation */
%token TOKEN_SEMI
%token TOKEN_COMMA
%token TOKEN_DOT
%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token TOKEN_LBRACE
%token TOKEN_RBRACE

/* Literals / identifiers */
%token TOKEN_IDENT
%token TOKEN_NUMBER
%token TOKEN_STRING

/* Precedence */
%right TOKEN_ASSIGN
%left TOKEN_OR
%left TOKEN_AND
%left TOKEN_EQ TOKEN_NEQ
%left TOKEN_LT TOKEN_LTE TOKEN_GT TOKEN_GTE
%left TOKEN_ADD TOKEN_SUB
%left TOKEN_MUL TOKEN_DIV TOKEN_MOD
%right TOKEN_NOT
%nonassoc LOWER_THAN_ELSE
%nonassoc TOKEN_ELSE

%%

program
    : include_list function_list
    ;

include_list
    : /* empty */
    | include_list include_stmt
    ;

include_stmt
    : TOKEN_INCLUDE TOKEN_LT TOKEN_IDENT TOKEN_GT
    ;

function_list
    : function
    | function_list function
    ;

function
    : TOKEN_VOID TOKEN_SETUP TOKEN_LPAREN TOKEN_RPAREN block
    | TOKEN_VOID TOKEN_LOOP TOKEN_LPAREN TOKEN_RPAREN block
    ;

block
    : TOKEN_LBRACE stmt_list_opt TOKEN_RBRACE
    ;

stmt_list_opt
    : /* empty */
    | stmt_list
    ;

stmt_list
    : stmt
    | stmt_list stmt
    ;

stmt
    : declaration_stmt
    | assignment_stmt
    | if_stmt
    | while_stmt
    | arduino_stmt
    | return_stmt
    | break_stmt
    | block
    ;

declaration_stmt
    : type TOKEN_IDENT TOKEN_SEMI
    | type TOKEN_IDENT TOKEN_ASSIGN expr TOKEN_SEMI
    ;

type
    : TOKEN_INT
    | TOKEN_BOOL
    ;

assignment_stmt
    : TOKEN_IDENT TOKEN_ASSIGN expr TOKEN_SEMI
    ;

if_stmt
    : TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN stmt %prec LOWER_THAN_ELSE
    | TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN stmt TOKEN_ELSE stmt
    ;

while_stmt
    : TOKEN_WHILE TOKEN_LPAREN expr TOKEN_RPAREN stmt
    ;

return_stmt
    : TOKEN_RETURN TOKEN_SEMI
    | TOKEN_RETURN expr TOKEN_SEMI
    ;

break_stmt
    : TOKEN_BREAK TOKEN_SEMI
    ;

arduino_stmt
    : serial_stmt
    | delay_stmt
    | pinmode_stmt
    | digitalwrite_stmt
    | digitalread_stmt
    | analogwrite_stmt
    | analogread_stmt
    ;

serial_stmt
    : TOKEN_SERIAL TOKEN_DOT TOKEN_PRINT TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_SEMI
    | TOKEN_SERIAL TOKEN_DOT TOKEN_PRINTLN TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_SEMI
    ;

delay_stmt
    : TOKEN_DELAY TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_SEMI
    ;

pinmode_stmt
    : TOKEN_PINMODE TOKEN_LPAREN expr TOKEN_COMMA expr TOKEN_RPAREN TOKEN_SEMI
    ;

digitalwrite_stmt
    : TOKEN_DIGITALWRITE TOKEN_LPAREN expr TOKEN_COMMA expr TOKEN_RPAREN TOKEN_SEMI
    ;

digitalread_stmt
    : TOKEN_DIGITALREAD TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_SEMI
    ;

analogwrite_stmt
    : TOKEN_ANALOGWRITE TOKEN_LPAREN expr TOKEN_COMMA expr TOKEN_RPAREN TOKEN_SEMI
    ;

analogread_stmt
    : TOKEN_ANALOGREAD TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_SEMI
    ;

expr
    : TOKEN_IDENT
    | TOKEN_NUMBER
    | TOKEN_STRING
    | TOKEN_TRUE
    | TOKEN_FALSE
    | TOKEN_LPAREN expr TOKEN_RPAREN
    | TOKEN_NOT expr
    | expr TOKEN_ADD expr
    | expr TOKEN_SUB expr
    | expr TOKEN_MUL expr
    | expr TOKEN_DIV expr
    | expr TOKEN_MOD expr
    | expr TOKEN_EQ expr
    | expr TOKEN_NEQ expr
    | expr TOKEN_LT expr
    | expr TOKEN_LTE expr
    | expr TOKEN_GT expr
    | expr TOKEN_GTE expr
    | expr TOKEN_AND expr
    | expr TOKEN_OR expr
    | TOKEN_DIGITALREAD TOKEN_LPAREN expr TOKEN_RPAREN
    | TOKEN_ANALOGREAD TOKEN_LPAREN expr TOKEN_RPAREN
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}