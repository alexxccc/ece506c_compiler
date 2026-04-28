%code requires {
#include "../SemanticRoutines/ast.h"
}

%{
#include <stdio.h>
#include <stdlib.h>

#include "../SemanticRoutines/ast.h"

extern int yylex(void);
extern FILE *yyin;
extern int yylineno;

ASTNode *g_ast_root = NULL;

void yyerror(const char *s);
%}

%union {
    char *string_value;
    ASTNode *node;
    ASTNodeList *list;
    TypeKind type;
}

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
%token <string_value> TOKEN_IDENT
%token <string_value> TOKEN_NUMBER
%token <string_value> TOKEN_STRING

%type <node> program include_stmt function block stmt declaration_stmt assignment_stmt if_stmt while_stmt
%type <node> return_stmt break_stmt arduino_stmt serial_stmt delay_stmt pinmode_stmt digitalwrite_stmt
%type <node> digitalread_stmt analogwrite_stmt analogread_stmt expr
%type <list> include_list function_list stmt_list stmt_list_opt
%type <type> type

/* Precedence */
%right TOKEN_ASSIGN
%left TOKEN_OR
%left TOKEN_AND
%left TOKEN_EQ TOKEN_NEQ
%left TOKEN_LT TOKEN_LTE TOKEN_GT TOKEN_GTE
%left TOKEN_ADD TOKEN_SUB
%left TOKEN_MUL TOKEN_DIV TOKEN_MOD
%right TOKEN_NOT

%%

program
    : include_list function_list
      {
          $$ = ast_make_program($1, $2, yylineno);
          g_ast_root = $$;
      }
    ;

include_list
    : /* empty */
      {
          $$ = NULL;
      }
    | include_list include_stmt
      {
          $$ = ast_list_append($1, $2);
      }
    ;

include_stmt
    : TOKEN_INCLUDE TOKEN_LT TOKEN_IDENT TOKEN_GT
      {
          $$ = ast_make_include($3, yylineno);
          free($3);
      }
    ;

function_list
    : function
      {
          $$ = ast_list_append(NULL, $1);
      }
    | function_list function
      {
          $$ = ast_list_append($1, $2);
      }
    ;

function
    : TOKEN_VOID TOKEN_SETUP TOKEN_LPAREN TOKEN_RPAREN block
      {
          $$ = ast_make_function("setup", TYPE_VOID, $5, yylineno);
      }
    | TOKEN_VOID TOKEN_LOOP TOKEN_LPAREN TOKEN_RPAREN block
      {
          $$ = ast_make_function("loop", TYPE_VOID, $5, yylineno);
      }
    ;

block
    : TOKEN_LBRACE stmt_list_opt TOKEN_RBRACE
      {
          $$ = ast_make_block($2, yylineno);
      }
    ;

stmt_list_opt
    : /* empty */
      {
          $$ = NULL;
      }
    | stmt_list
      {
          $$ = $1;
      }
    ;

stmt_list
    : stmt
      {
          $$ = ast_list_append(NULL, $1);
      }
    | stmt_list stmt
      {
          $$ = ast_list_append($1, $2);
      }
    ;

stmt
    : declaration_stmt
      {
          $$ = $1;
      }
    | assignment_stmt
      {
          $$ = $1;
      }
    | if_stmt
      {
          $$ = $1;
      }
    | while_stmt
      {
          $$ = $1;
      }
    | arduino_stmt
      {
          $$ = $1;
      }
    | return_stmt
      {
          $$ = $1;
      }
    | break_stmt
      {
          $$ = $1;
      }
    | block
      {
          $$ = $1;
      }
    ;

declaration_stmt
    : type TOKEN_IDENT TOKEN_SEMI
      {
          $$ = ast_make_declaration($1, $2, NULL, yylineno);
          free($2);
      }
    | type TOKEN_IDENT TOKEN_ASSIGN expr TOKEN_SEMI
      {
          $$ = ast_make_declaration($1, $2, $4, yylineno);
          free($2);
      }
    ;

type
    : TOKEN_INT
      {
          $$ = TYPE_INT;
      }
    | TOKEN_BOOL
      {
          $$ = TYPE_BOOL;
      }
    ;

assignment_stmt
    : TOKEN_IDENT TOKEN_ASSIGN expr TOKEN_SEMI
      {
          $$ = ast_make_assignment($1, $3, yylineno);
          free($1);
      }
    ;

if_stmt
    : TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN stmt
      {
          $$ = ast_make_if($3, $5, NULL, yylineno);
      }
    | TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN stmt TOKEN_ELSE stmt
      {
          $$ = ast_make_if($3, $5, $7, yylineno);
      }
    ;

while_stmt
    : TOKEN_WHILE TOKEN_LPAREN expr TOKEN_RPAREN stmt
      {
          $$ = ast_make_while($3, $5, yylineno);
      }
    ;

return_stmt
    : TOKEN_RETURN TOKEN_SEMI
      {
          $$ = ast_make_return(NULL, yylineno);
      }
    | TOKEN_RETURN expr TOKEN_SEMI
      {
          $$ = ast_make_return($2, yylineno);
      }
    ;

break_stmt
    : TOKEN_BREAK TOKEN_SEMI
      {
          $$ = ast_make_break(yylineno);
      }
    ;

arduino_stmt
    : serial_stmt
      {
          $$ = $1;
      }
    | delay_stmt
      {
          $$ = $1;
      }
    | pinmode_stmt
      {
          $$ = $1;
      }
    | digitalwrite_stmt
      {
          $$ = $1;
      }
    | digitalread_stmt
      {
          $$ = $1;
      }
    | analogwrite_stmt
      {
          $$ = $1;
      }
    | analogread_stmt
      {
          $$ = $1;
      }
    ;

serial_stmt
    : TOKEN_SERIAL TOKEN_DOT TOKEN_PRINT TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_SEMI
      {
          $$ = ast_make_arduino_call(CALL_SERIAL_PRINT, $5, NULL, yylineno);
      }
    | TOKEN_SERIAL TOKEN_DOT TOKEN_PRINTLN TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_SEMI
      {
          $$ = ast_make_arduino_call(CALL_SERIAL_PRINTLN, $5, NULL, yylineno);
      }
    ;

delay_stmt
    : TOKEN_DELAY TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_SEMI
      {
          $$ = ast_make_arduino_call(CALL_DELAY, $3, NULL, yylineno);
      }
    ;

pinmode_stmt
    : TOKEN_PINMODE TOKEN_LPAREN expr TOKEN_COMMA expr TOKEN_RPAREN TOKEN_SEMI
      {
          $$ = ast_make_arduino_call(CALL_PINMODE, $3, $5, yylineno);
      }
    ;

digitalwrite_stmt
    : TOKEN_DIGITALWRITE TOKEN_LPAREN expr TOKEN_COMMA expr TOKEN_RPAREN TOKEN_SEMI
      {
          $$ = ast_make_arduino_call(CALL_DIGITAL_WRITE, $3, $5, yylineno);
      }
    ;

digitalread_stmt
    : TOKEN_DIGITALREAD TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_SEMI
      {
          $$ = ast_make_arduino_call(CALL_DIGITAL_READ, $3, NULL, yylineno);
      }
    ;

analogwrite_stmt
    : TOKEN_ANALOGWRITE TOKEN_LPAREN expr TOKEN_COMMA expr TOKEN_RPAREN TOKEN_SEMI
      {
          $$ = ast_make_arduino_call(CALL_ANALOG_WRITE, $3, $5, yylineno);
      }
    ;

analogread_stmt
    : TOKEN_ANALOGREAD TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_SEMI
      {
          $$ = ast_make_arduino_call(CALL_ANALOG_READ, $3, NULL, yylineno);
      }
    ;

expr
    : TOKEN_IDENT
      {
          $$ = ast_make_identifier($1, yylineno);
          free($1);
      }
    | TOKEN_NUMBER
      {
          $$ = ast_make_number_literal($1, yylineno);
          free($1);
      }
    | TOKEN_STRING
      {
          $$ = ast_make_string_literal($1, yylineno);
          free($1);
      }
    | TOKEN_TRUE
      {
          $$ = ast_make_bool_literal(1, yylineno);
      }
    | TOKEN_FALSE
      {
          $$ = ast_make_bool_literal(0, yylineno);
      }
    | TOKEN_LPAREN expr TOKEN_RPAREN
      {
          $$ = $2;
      }
    | TOKEN_NOT expr
      {
          $$ = ast_make_unary(OP_NOT, $2, yylineno);
      }
    | expr TOKEN_ADD expr
      {
          $$ = ast_make_binary(OP_ADD, $1, $3, yylineno);
      }
    | expr TOKEN_SUB expr
      {
          $$ = ast_make_binary(OP_SUB, $1, $3, yylineno);
      }
    | expr TOKEN_MUL expr
      {
          $$ = ast_make_binary(OP_MUL, $1, $3, yylineno);
      }
    | expr TOKEN_DIV expr
      {
          $$ = ast_make_binary(OP_DIV, $1, $3, yylineno);
      }
    | expr TOKEN_MOD expr
      {
          $$ = ast_make_binary(OP_MOD, $1, $3, yylineno);
      }
    | expr TOKEN_EQ expr
      {
          $$ = ast_make_binary(OP_EQ, $1, $3, yylineno);
      }
    | expr TOKEN_NEQ expr
      {
          $$ = ast_make_binary(OP_NEQ, $1, $3, yylineno);
      }
    | expr TOKEN_LT expr
      {
          $$ = ast_make_binary(OP_LT, $1, $3, yylineno);
      }
    | expr TOKEN_LTE expr
      {
          $$ = ast_make_binary(OP_LTE, $1, $3, yylineno);
      }
    | expr TOKEN_GT expr
      {
          $$ = ast_make_binary(OP_GT, $1, $3, yylineno);
      }
    | expr TOKEN_GTE expr
      {
          $$ = ast_make_binary(OP_GTE, $1, $3, yylineno);
      }
    | expr TOKEN_AND expr
      {
          $$ = ast_make_binary(OP_AND, $1, $3, yylineno);
      }
    | expr TOKEN_OR expr
      {
          $$ = ast_make_binary(OP_OR, $1, $3, yylineno);
      }
    | TOKEN_DIGITALREAD TOKEN_LPAREN expr TOKEN_RPAREN
      {
          $$ = ast_make_arduino_call(CALL_DIGITAL_READ, $3, NULL, yylineno);
      }
    | TOKEN_ANALOGREAD TOKEN_LPAREN expr TOKEN_RPAREN
      {
          $$ = ast_make_arduino_call(CALL_ANALOG_READ, $3, NULL, yylineno);
      }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error on line %d: %s\n", yylineno, s);
}
