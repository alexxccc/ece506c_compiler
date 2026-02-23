%{
#include <stdio.h>
#include "token.h"

/*
 Flex scanner for Arduino-sketch subset.
 Key features:
  - Skips whitespace and // comments
  - Returns TOKEN_ERROR for any non-matching character
*/
%}

DIGIT      [0-9]
LETTER     [A-Za-z]
ID         {LETTER}({LETTER}|{DIGIT}|_)*
NUMBER     {DIGIT}+
WS         [ \t\r\n]+
COMMENT    "//".*

STRING     \"([^"\\\n]|\\.)*\"

%%

{WS}                 { /* skip whitespace */ }
{COMMENT}            { /* skip comment */ }

"#include"           { return TOKEN_INCLUDE; }
"void"               { return TOKEN_VOID; }
"return"             { return TOKEN_RETURN; }
"int"                { return TOKEN_INT; }
"bool"               { return TOKEN_BOOL; }
"if"                 { return TOKEN_IF; }
"else"               { return TOKEN_ELSE; }
"while"              { return TOKEN_WHILE; }
"true"               { return TOKEN_TRUE; }
"false"              { return TOKEN_FALSE; }
"break"              { return TOKEN_BREAK; }

"Serial"             { return TOKEN_SERIAL; }
"print"              { return TOKEN_PRINT; }
"println"            { return TOKEN_PRINTLN; }
"setup"              { return TOKEN_SETUP; }
"loop"               { return TOKEN_LOOP; }
"delay"              { return TOKEN_DELAY; }
"pinMode"            { return TOKEN_PINMODE; }
"analogRead"         { return TOKEN_ANALOGREAD; }
"analogWrite"        { return TOKEN_ANALOGWRITE; }
"digitalRead"        { return TOKEN_DIGITALREAD; }
"digitalWrite"       { return TOKEN_DIGITALWRITE; }

"=="                 { return TOKEN_EQ; }
">="                 { return TOKEN_GTE; }
"<="                 { return TOKEN_LTE; }
"!="                 { return TOKEN_NEQ; }
">"                  { return TOKEN_GT; }
"<"                  { return TOKEN_LT; }

"&&"                 { return TOKEN_AND; }
"||"                 { return TOKEN_OR; }
"!"                  { return TOKEN_NOT; }

"="                  { return TOKEN_ASSIGN; }
"+"                  { return TOKEN_ADD; }
"-"                  { return TOKEN_SUB; }
"*"                  { return TOKEN_MUL; }
"/"                  { return TOKEN_DIV; }
"%"                  { return TOKEN_MOD; }

";"                  { return TOKEN_SEMI; }
","                  { return TOKEN_COMMA; }
"."                  { return TOKEN_DOT; }
"("                  { return TOKEN_LPAREN; }
")"                  { return TOKEN_RPAREN; }
"{"                  { return TOKEN_LBRACE; }
"}"                  { return TOKEN_RBRACE; }

{STRING}             { return TOKEN_STRING; }
{NUMBER}             { return TOKEN_NUMBER; }
{ID}                 { return TOKEN_IDENT; }

.                    { return TOKEN_ERROR; }

%%

int yywrap(void) { return 1; }
