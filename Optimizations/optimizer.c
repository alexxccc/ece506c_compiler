#include "optimizer.h"

#include <stdio.h>
#include <string.h>

#include "algebraic_strength_reduction.h"
#include "constant_folding.h"
#include "constant_propagation.h"
#include "dead_code_elimination.h"

static const unsigned int ALL_OPTIMIZATIONS =
    OPTIMIZATION_CONSTANT_PROPAGATION
    | OPTIMIZATION_ALGEBRAIC_STRENGTH_REDUCTION
    | OPTIMIZATION_CONSTANT_FOLDING
    | OPTIMIZATION_DEAD_CODE_ELIMINATION;

OptimizationOptions optimization_options_all(void) {
    // Turn on all optimizations
    OptimizationOptions options;
    options.flags = ALL_OPTIMIZATIONS;
    return options;
}

OptimizationOptions optimization_options_none(void) {
    // Turn off all optimizations
    OptimizationOptions options;
    options.flags = 0;
    return options;
}

int optimization_enable_by_name(OptimizationOptions *options, const char *name) {
    // If name matches optimization, turn on optimization in options
    if (options == NULL || name == NULL) {
        return 0;
    }

    if (strcmp(name, "constant-folding") == 0 || strcmp(name, "const-fold") == 0) {
        options->flags |= OPTIMIZATION_CONSTANT_FOLDING;
        return 1;
    }

    if (strcmp(name, "constant-propagation") == 0 || strcmp(name, "const-prop") == 0) {
        options->flags |= OPTIMIZATION_CONSTANT_PROPAGATION;
        return 1;
    }

    if (strcmp(name, "dead-code-elimination") == 0 || strcmp(name, "dce") == 0) {
        options->flags |= OPTIMIZATION_DEAD_CODE_ELIMINATION;
        return 1;
    }

    if (strcmp(name, "algebraic-strength-reduction") == 0
        || strcmp(name, "strength-reduction") == 0
        || strcmp(name, "asr") == 0) {
        options->flags |= OPTIMIZATION_ALGEBRAIC_STRENGTH_REDUCTION;
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
    // If name matches optimization, turn off optimization in options
    if (options == NULL || name == NULL) {
        return 0;
    }

    if (strcmp(name, "constant-folding") == 0 || strcmp(name, "const-fold") == 0) {
        options->flags &= ~OPTIMIZATION_CONSTANT_FOLDING;
        return 1;
    }

    if (strcmp(name, "constant-propagation") == 0 || strcmp(name, "const-prop") == 0) {
        options->flags &= ~OPTIMIZATION_CONSTANT_PROPAGATION;
        return 1;
    }

    if (strcmp(name, "dead-code-elimination") == 0 || strcmp(name, "dce") == 0) {
        options->flags &= ~OPTIMIZATION_DEAD_CODE_ELIMINATION;
        return 1;
    }

    if (strcmp(name, "algebraic-strength-reduction") == 0
        || strcmp(name, "strength-reduction") == 0
        || strcmp(name, "asr") == 0) {
        options->flags &= ~OPTIMIZATION_ALGEBRAIC_STRENGTH_REDUCTION;
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

    fprintf(out, "constant-propagation\n");
    fprintf(out, "algebraic-strength-reduction\n");
    fprintf(out, "constant-folding\n");
    fprintf(out, "dead-code-elimination\n");
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

    if ((options->flags & OPTIMIZATION_CONSTANT_PROPAGATION) != 0) {
        fprintf(out, "constant-propagation");
        printed = 1;
    }

    if ((options->flags & OPTIMIZATION_ALGEBRAIC_STRENGTH_REDUCTION) != 0) {
        fprintf(out, "%salgebraic-strength-reduction", printed ? ", " : "");
        printed = 1;
    }

    if ((options->flags & OPTIMIZATION_CONSTANT_FOLDING) != 0) {
        fprintf(out, "%sconstant-folding", printed ? ", " : "");
        printed = 1;
    }

    if ((options->flags & OPTIMIZATION_DEAD_CODE_ELIMINATION) != 0) {
        fprintf(out, "%sdead-code-elimination", printed ? ", " : "");
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

    if ((options->flags & OPTIMIZATION_CONSTANT_PROPAGATION) != 0) {
        root = propagate_constants(root);
    }

    if ((options->flags & OPTIMIZATION_ALGEBRAIC_STRENGTH_REDUCTION) != 0) {
        root = reduce_algebraic_strength(root);
    }

    if ((options->flags & OPTIMIZATION_CONSTANT_FOLDING) != 0) {
        root = fold_constants(root);
    }

    if ((options->flags & OPTIMIZATION_DEAD_CODE_ELIMINATION) != 0) {
        root = eliminate_dead_code(root);
    }

    return root;
}
