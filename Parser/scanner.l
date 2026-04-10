%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.tab.h"

static char *copy_lexeme(const char *text) {
    size_t length = strlen(text);
    char *copy = (char *)malloc(length + 1);
    if (copy == NULL) {
        fprintf(stderr, "Out of memory while copying scanner lexeme.\n");
        exit(EXIT_FAILURE);
    }
    memcpy(copy, text, length + 1);
    return copy;
}
%}

%option noyywrap
%option yylineno

DIGIT       [0-9]
LETTER      [A-Za-z]
IDENT       {LETTER}({LETTER}|{DIGIT}|_)*
NUMBER      {DIGIT}+
STRING      \"([^\\\"\n]|\\.)*\"

%%

[ \t\r\n]+                      ;
"//".*                          ;

"#include"                      { return TOKEN_INCLUDE; }
"void"                          { return TOKEN_VOID; }
"return"                        { return TOKEN_RETURN; }
"int"                           { return TOKEN_INT; }
"bool"                          { return TOKEN_BOOL; }
"if"                            { return TOKEN_IF; }
"else"                          { return TOKEN_ELSE; }
"while"                         { return TOKEN_WHILE; }
"true"                          { return TOKEN_TRUE; }
"false"                         { return TOKEN_FALSE; }
"break"                         { return TOKEN_BREAK; }

"Serial"                        { return TOKEN_SERIAL; }
"print"                         { return TOKEN_PRINT; }
"println"                       { return TOKEN_PRINTLN; }
"setup"                         { return TOKEN_SETUP; }
"loop"                          { return TOKEN_LOOP; }
"delay"                         { return TOKEN_DELAY; }
"pinMode"                       { return TOKEN_PINMODE; }
"analogRead"                    { return TOKEN_ANALOGREAD; }
"analogWrite"                   { return TOKEN_ANALOGWRITE; }
"digitalRead"                   { return TOKEN_DIGITALREAD; }
"digitalWrite"                  { return TOKEN_DIGITALWRITE; }

"=="                            { return TOKEN_EQ; }
"!="                            { return TOKEN_NEQ; }
">="                            { return TOKEN_GTE; }
"<="                            { return TOKEN_LTE; }
"&&"                            { return TOKEN_AND; }
"||"                            { return TOKEN_OR; }

"="                             { return TOKEN_ASSIGN; }
"+"                             { return TOKEN_ADD; }
"-"                             { return TOKEN_SUB; }
"*"                             { return TOKEN_MUL; }
"/"                             { return TOKEN_DIV; }
"%"                             { return TOKEN_MOD; }
">"                             { return TOKEN_GT; }
"<"                             { return TOKEN_LT; }
"!"                             { return TOKEN_NOT; }

";"                             { return TOKEN_SEMI; }
","                             { return TOKEN_COMMA; }
"."                             { return TOKEN_DOT; }
"("                             { return TOKEN_LPAREN; }
")"                             { return TOKEN_RPAREN; }
"{"                             { return TOKEN_LBRACE; }
"}"                             { return TOKEN_RBRACE; }

{STRING}                        { yylval.string_value = copy_lexeme(yytext); return TOKEN_STRING; }
{NUMBER}                        { yylval.string_value = copy_lexeme(yytext); return TOKEN_NUMBER; }
{IDENT}                         { yylval.string_value = copy_lexeme(yytext); return TOKEN_IDENT; }

.                               { return TOKEN_ERROR; }

%%
