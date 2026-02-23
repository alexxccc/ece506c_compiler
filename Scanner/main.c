#include "token.h"
#include <stdio.h>
#include <stdlib.h>

extern FILE *yyin;
extern int yylex(void);
extern char *yytext;

int main(int argc, char **argv) {
    // updated to file names for different tests. Default is program.c
    const char *filename = (argc >= 2) ? argv[1] : "tests/program.c";

    yyin = fopen(filename, "r");
    if (!yyin) {
        fprintf(stderr, "could not open %s!\n", filename);
        return 1;
    }

    int tok;
    while ((tok = yylex()) != 0) {  /* 0 means EOF in Flex */
        printf("token: %d text: %s\n", tok, yytext);
    }

    fclose(yyin);
    return 0;
}