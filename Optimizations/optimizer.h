#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <stdio.h>

#include "../SemanticRoutines/ast.h"

typedef enum {
    OPTIMIZATION_CONSTANT_FOLDING = 1 << 0,
    OPTIMIZATION_CONSTANT_PROPAGATION = 1 << 1,
    OPTIMIZATION_DEAD_CODE_ELIMINATION = 1 << 2,
    OPTIMIZATION_ALGEBRAIC_STRENGTH_REDUCTION = 1 << 3
} OptimizationFlag;

typedef struct {
    unsigned int flags;
} OptimizationOptions;

OptimizationOptions optimization_options_all(void);
OptimizationOptions optimization_options_none(void);

int optimization_enable_by_name(OptimizationOptions *options, const char *name);
int optimization_disable_by_name(OptimizationOptions *options, const char *name);
void optimization_print_available(FILE *out);
void optimization_print_enabled(const OptimizationOptions *options, FILE *out);

ASTNode *optimize_ast(ASTNode *root, const OptimizationOptions *options);

#endif
