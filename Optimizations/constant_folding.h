#ifndef CONSTANT_FOLDING_H
#define CONSTANT_FOLDING_H

#include "../SemanticRoutines/ast.h"

ASTNode *fold_constants(ASTNode *root);

#endif
