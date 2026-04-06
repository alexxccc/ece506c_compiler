%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"
%}

%option noyywrap

DIGIT       [0-9]
LETTER      [A-Za-z]
IDENT       {LETTER}({LETTER}|{DIGIT}|_)*
NUMBER      {DIGIT}+
STRING      \"([^\\\"\n]|\\.)*\"

%%

[ \t\r\n]+                      ;

"//".*                          ;

/* C / Arduino keywords */
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

/* Arduino-specific keywords */
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

/* Multi-character operators first */
"=="                            { return TOKEN_EQ; }
"!="                            { return TOKEN_NEQ; }
">="                            { return TOKEN_GTE; }
"<="                            { return TOKEN_LTE; }
"&&"                            { return TOKEN_AND; }
"||"                            { return TOKEN_OR; }

/* Single-character operators */
"="                             { return TOKEN_ASSIGN; }
"+"                             { return TOKEN_ADD; }
"-"                             { return TOKEN_SUB; }
"*"                             { return TOKEN_MUL; }
"/"                             { return TOKEN_DIV; }
"%"                             { return TOKEN_MOD; }
">"                             { return TOKEN_GT; }
"<"                             { return TOKEN_LT; }
"!"                             { return TOKEN_NOT; }

/* Punctuation */
";"                             { return TOKEN_SEMI; }
","                             { return TOKEN_COMMA; }
"."                             { return TOKEN_DOT; }
"("                             { return TOKEN_LPAREN; }
")"                             { return TOKEN_RPAREN; }
"{"                             { return TOKEN_LBRACE; }
"}"                             { return TOKEN_RBRACE; }

/* Literals and identifiers */
{STRING}                        { return TOKEN_STRING; }
{NUMBER}                        { return TOKEN_NUMBER; }
{IDENT}                         { return TOKEN_IDENT; }

/* Anything else is invalid */
.                               { return TOKEN_ERROR; }

%%