#include <stdio.h>
#include <stdlib.h>

#include "../SemanticRoutines/semantic.h"

extern FILE *yyin;
extern int yyparse(void);
extern ASTNode *g_ast_root;

int main(int argc, char **argv) {
    const char *filename = (argc >= 2) ? argv[1] : "tests/pass/test1_basic.c";
    int parse_result;
    int semantic_errors;
    SymbolTable *symbol_table;

    yyin = fopen(filename, "r");
    if (!yyin) {
        fprintf(stderr, "Could not open %s\n", filename);
        return 1;
    }

    parse_result = yyparse();
    fclose(yyin);

    if (parse_result != 0) {
        printf("Parse failed: %s\n", filename);
        return 1;
    }

    symbol_table = analyze_program(g_ast_root);

    printf("Parse successful: %s\n\n", filename);
    printf("AST (post-order traversal)\n");
    ast_print_post_order(g_ast_root, stdout);
    printf("\n");

    print_semantic_errors(symbol_table, stdout);
    printf("\n");
    print_symbol_tables(symbol_table, stdout);

    semantic_errors = semantic_error_count(symbol_table);

    free_symbol_table(symbol_table);
    ast_free(g_ast_root);
    g_ast_root = NULL;

    return (semantic_errors == 0) ? 0 : 1;
}
