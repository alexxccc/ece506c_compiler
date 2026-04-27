#ifndef DEAD_CODE_ELIMINATION_H
#define DEAD_CODE_ELIMINATION_H

#include "../SemanticRoutines/ast.h"

ASTNode *eliminate_dead_code(ASTNode *root);

#endif
