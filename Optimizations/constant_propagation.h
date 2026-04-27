#ifndef CONSTANT_PROPAGATION_H
#define CONSTANT_PROPAGATION_H

#include "../SemanticRoutines/ast.h"

ASTNode *propagate_constants(ASTNode *root);

#endif
