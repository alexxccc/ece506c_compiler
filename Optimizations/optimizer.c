#include "optimizer.h"

#include <stdio.h>
#include <string.h>

#include "constant_folding.h"

static const unsigned int ALL_OPTIMIZATIONS = OPTIMIZATION_CONSTANT_FOLDING;

OptimizationOptions optimization_options_all(void) {
    OptimizationOptions options;
    options.flags = ALL_OPTIMIZATIONS;
    return options;
}

OptimizationOptions optimization_options_none(void) {
    OptimizationOptions options;
    options.flags = 0;
    return options;
}

int optimization_enable_by_name(OptimizationOptions *options, const char *name) {
    if (options == NULL || name == NULL) {
        return 0;
    }

    if (strcmp(name, "constant-folding") == 0 || strcmp(name, "const-fold") == 0) {
        options->flags |= OPTIMIZATION_CONSTANT_FOLDING;
        return 1;
    }

    if (strcmp(name, "all") == 0) {
        options->flags |= ALL_OPTIMIZATIONS;
        return 1;
    }

    if (strcmp(name, "none") == 0) {
        options->flags = 0;
        return 1;
    }

    return 0;
}

int optimization_disable_by_name(OptimizationOptions *options, const char *name) {
    if (options == NULL || name == NULL) {
        return 0;
    }

    if (strcmp(name, "constant-folding") == 0 || strcmp(name, "const-fold") == 0) {
        options->flags &= ~OPTIMIZATION_CONSTANT_FOLDING;
        return 1;
    }

    if (strcmp(name, "all") == 0) {
        options->flags = 0;
        return 1;
    }

    return 0;
}

void optimization_print_available(FILE *out) {
    if (out == NULL) {
        return;
    }

    fprintf(out, "constant-folding\n");
}

void optimization_print_enabled(const OptimizationOptions *options, FILE *out) {
    int printed = 0;

    if (out == NULL) {
        return;
    }

    if (options == NULL || options->flags == 0) {
        fprintf(out, "none");
        return;
    }

    if ((options->flags & OPTIMIZATION_CONSTANT_FOLDING) != 0) {
        fprintf(out, "constant-folding");
        printed = 1;
    }

    if (!printed) {
        fprintf(out, "none");
    }
}

ASTNode *optimize_ast(ASTNode *root, const OptimizationOptions *options) {
    if (root == NULL || options == NULL) {
        return root;
    }

    if ((options->flags & OPTIMIZATION_CONSTANT_FOLDING) != 0) {
        root = fold_constants(root);
    }

    return root;
}
