#include <stdio.h>
#include <stdlib.h>

extern FILE *yyin;
extern int yyparse(void);

int main(int argc, char **argv) {
    const char *filename = (argc >= 2) ? argv[1] : "tests/pass/test1.txt";

    yyin = fopen(filename, "r");
    if (!yyin) {
        fprintf(stderr, "Could not open %s\n", filename);
        return 1;
    }

    int result = yyparse();
    fclose(yyin);

    if (result == 0) {
        printf("Parse successful: %s\n", filename);
        return 0;
    } else {
        printf("Parse failed: %s\n", filename);
        return 1;
    }
}