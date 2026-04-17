#include <stdio.h>
#include <stdlib.h>

#include "../SemanticRoutines/ast.h"

int main(int argc, char **argv) {
    const char *input_path = (argc >= 2) ? argv[1] : "ast.txt";
    ASTNode *root = ast_read_post_order_file(input_path);

    if (root == NULL && argc < 2) {
        input_path = "../Parser/ast.txt";
        root = ast_read_post_order_file(input_path);
    }

    if (root == NULL) {
        fprintf(stderr, "Could not reconstruct AST from %s\n", input_path);
        return 1;
    }

    printf("Reconstructed AST from %s\n", input_path);
    printf("AST (post-order traversal)\n");
    ast_print_post_order(root, stdout);
    printf("\n");

    ast_free(root);
    return 0;
}
