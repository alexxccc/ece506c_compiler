#include "constant_propagation.h"

#include <stdlib.h>
#include <string.h>

// Used for quick access to variable values for replacement
typedef struct ConstantBinding {
    char *name;
    ASTNode *literal;
    struct ConstantBinding *next;
} ConstantBinding;

typedef struct AssignedName {
    char *name;
    struct AssignedName *next;
} AssignedName;

static ASTNode *propagate_node(ASTNode *node, ConstantBinding **bindings);

static char *copy_string(const char *text) {
    size_t length;
    char *copy;

    if (text == NULL) {
        return NULL;
    }

    length = strlen(text);
    copy = (char *)malloc(length + 1);
    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, text, length + 1);
    return copy;
}

static int is_constant_literal(const ASTNode *node) {
    return node != NULL
        && (node->kind == AST_NUMBER_LITERAL || node->kind == AST_BOOL_LITERAL);
}

static ASTNode *clone_constant_literal(const ASTNode *literal) {
    ASTNode *copy;

    if (!is_constant_literal(literal)) {
        return NULL;
    }

    if (literal->kind == AST_NUMBER_LITERAL) {
        copy = ast_make_number_literal(literal->data.literal.value, literal->line);
    } else {
        copy = ast_make_bool_literal(literal->data.literal.bool_value, literal->line);
    }

    copy->inferred_type = literal->inferred_type;
    return copy;
}

static void free_binding(ConstantBinding *binding) {
    if (binding == NULL) {
        return;
    }

    free(binding->name);
    ast_free(binding->literal);
    free(binding);
}

static void free_bindings(ConstantBinding *bindings) {
    while (bindings != NULL) {
        ConstantBinding *next = bindings->next;
        free_binding(bindings);
        bindings = next;
    }
}

// Determine if a variable already has a binding
static ConstantBinding *find_binding(ConstantBinding *bindings, const char *name) {
    while (bindings != NULL) {
        if (strcmp(bindings->name, name) == 0) {
            return bindings;
        }
        bindings = bindings->next;
    }

    return NULL;
}

// Alter bindings (if a variable value changes)
static void remove_binding(ConstantBinding **bindings, const char *name) {
    ConstantBinding *cursor;
    ConstantBinding *previous = NULL;

    if (bindings == NULL || name == NULL) {
        return;
    }

    cursor = *bindings;
    while (cursor != NULL) {
        if (strcmp(cursor->name, name) == 0) {
            if (previous == NULL) {
                *bindings = cursor->next;
            } else {
                previous->next = cursor->next;
            }
            free_binding(cursor);
            return;
        }

        previous = cursor;
        cursor = cursor->next;
    }
}

static void set_binding(ConstantBinding **bindings, const char *name, const ASTNode *literal) {
    ConstantBinding *binding;
    ASTNode *literal_copy;

    if (bindings == NULL || name == NULL || !is_constant_literal(literal)) {
        return;
    }

    literal_copy = clone_constant_literal(literal);
    if (literal_copy == NULL) {
        return;
    }

    binding = find_binding(*bindings, name);
    if (binding != NULL) {
        ast_free(binding->literal);
        binding->literal = literal_copy;
        return;
    }

    binding = (ConstantBinding *)malloc(sizeof(ConstantBinding));
    if (binding == NULL) {
        ast_free(literal_copy);
        return;
    }

    binding->name = copy_string(name);
    if (binding->name == NULL) {
        ast_free(literal_copy);
        free(binding);
        return;
    }

    binding->literal = literal_copy;
    binding->next = *bindings;
    *bindings = binding;
}

static ConstantBinding *clone_bindings(ConstantBinding *bindings) {
    ConstantBinding *copy = NULL;
    ConstantBinding *cursor = bindings;

    while (cursor != NULL) {
        set_binding(&copy, cursor->name, cursor->literal);
        cursor = cursor->next;
    }

    return copy;
}

// AssignedName forms a list of the variables that have been assigned
static int assigned_name_contains(AssignedName *names, const char *name) {
    while (names != NULL) {
        if (strcmp(names->name, name) == 0) {
            return 1;
        }
        names = names->next;
    }

    return 0;
}

static void add_assigned_name(AssignedName **names, const char *name) {
    AssignedName *entry;

    if (names == NULL || name == NULL || assigned_name_contains(*names, name)) {
        return;
    }

    entry = (AssignedName *)malloc(sizeof(AssignedName));
    if (entry == NULL) {
        return;
    }

    entry->name = copy_string(name);
    if (entry->name == NULL) {
        free(entry);
        return;
    }

    entry->next = *names;
    *names = entry;
}

static void free_assigned_names(AssignedName *names) {
    while (names != NULL) {
        AssignedName *next = names->next;
        free(names->name);
        free(names);
        names = next;
    }
}

static void collect_assigned_names(ASTNode *node, AssignedName **names);

static void collect_assigned_names_from_list(ASTNodeList *list, AssignedName **names) {
    while (list != NULL) {
        collect_assigned_names(list->node, names);
        list = list->next;
    }
}

static void collect_assigned_names(ASTNode *node, AssignedName **names) {
    if (node == NULL) {
        return;
    }

    switch (node->kind) {
        case AST_PROGRAM:
            collect_assigned_names_from_list(node->data.program.functions, names);
            break;
        case AST_FUNCTION:
            collect_assigned_names(node->data.function.body, names);
            break;
        case AST_BLOCK:
            collect_assigned_names_from_list(node->data.block.statements, names);
            break;
        case AST_ASSIGNMENT:
            add_assigned_name(names, node->data.assignment.name);
            break;
        case AST_IF_STMT:
            collect_assigned_names(node->data.if_stmt.then_branch, names);
            collect_assigned_names(node->data.if_stmt.else_branch, names);
            break;
        case AST_WHILE_STMT:
            collect_assigned_names(node->data.while_stmt.body, names);
            break;
        default:
            break;
    }
}

static void remove_assigned_bindings(ConstantBinding **bindings, AssignedName *names) {
    while (names != NULL) {
        remove_binding(bindings, names->name);
        names = names->next;
    }
}

// Check each node. If it has a binding, return the associated value. Otherwise make no changes.
static ASTNode *propagate_identifier(ASTNode *node, ConstantBinding **bindings) {
    ConstantBinding *binding;
    ASTNode *replacement;

    if (node == NULL || node->kind != AST_IDENTIFIER || bindings == NULL) {
        return node;
    }

    binding = find_binding(*bindings, node->data.identifier.name);
    if (binding == NULL) {
        return node;
    }

    replacement = clone_constant_literal(binding->literal);
    if (replacement == NULL) {
        return node;
    }

    ast_free(node);
    return replacement;
}

static void propagate_list(ASTNodeList *list, ConstantBinding **bindings) {
    while (list != NULL) {
        list->node = propagate_node(list->node, bindings);
        list = list->next;
    }
}

// Remove variables that are assigned in if()/while(); may or may not have changed values
static ASTNode *propagate_if(ASTNode *node, ConstantBinding **bindings) {
    ConstantBinding *then_bindings;
    ConstantBinding *else_bindings;
    AssignedName *assigned_names = NULL;

    node->data.if_stmt.condition = propagate_node(node->data.if_stmt.condition, bindings);

    then_bindings = clone_bindings(*bindings);
    else_bindings = clone_bindings(*bindings);

    node->data.if_stmt.then_branch = propagate_node(node->data.if_stmt.then_branch, &then_bindings);
    node->data.if_stmt.else_branch = propagate_node(node->data.if_stmt.else_branch, &else_bindings);

    collect_assigned_names(node->data.if_stmt.then_branch, &assigned_names);
    collect_assigned_names(node->data.if_stmt.else_branch, &assigned_names);
    remove_assigned_bindings(bindings, assigned_names);

    free_assigned_names(assigned_names);
    free_bindings(then_bindings);
    free_bindings(else_bindings);
    return node;
}

static ASTNode *propagate_while(ASTNode *node, ConstantBinding **bindings) {
    ConstantBinding *body_bindings;
    AssignedName *assigned_names = NULL;

    node->data.while_stmt.condition = propagate_node(node->data.while_stmt.condition, bindings);

    collect_assigned_names(node->data.while_stmt.body, &assigned_names);
    body_bindings = clone_bindings(*bindings);
    remove_assigned_bindings(&body_bindings, assigned_names);
    node->data.while_stmt.body = propagate_node(node->data.while_stmt.body, &body_bindings);

    remove_assigned_bindings(bindings, assigned_names);

    free_assigned_names(assigned_names);
    free_bindings(body_bindings);
    return node;
}

// Check each node. Replace assignments to unchanged variables with assignments to constants, according to bindings.
static ASTNode *propagate_node(ASTNode *node, ConstantBinding **bindings) {
    if (node == NULL) {
        return NULL;
    }

    switch (node->kind) {
        case AST_PROGRAM:
            propagate_list(node->data.program.functions, bindings);
            break;
        case AST_FUNCTION: {
            ConstantBinding *function_bindings = NULL;
            if (node->data.function.body != NULL && node->data.function.body->kind == AST_BLOCK) {
                propagate_list(node->data.function.body->data.block.statements, &function_bindings);
            } else {
                node->data.function.body = propagate_node(node->data.function.body, &function_bindings);
            }
            free_bindings(function_bindings);
            break;
        }
        case AST_BLOCK: {
            ConstantBinding *block_bindings = (bindings == NULL) ? NULL : clone_bindings(*bindings);
            propagate_list(node->data.block.statements, &block_bindings);
            free_bindings(block_bindings);
            break;
        }
        case AST_DECLARATION:
            node->data.declaration.initializer = propagate_node(node->data.declaration.initializer, bindings);
            if (is_constant_literal(node->data.declaration.initializer)) {
                set_binding(bindings, node->data.declaration.name, node->data.declaration.initializer);
            } else {
                remove_binding(bindings, node->data.declaration.name);
            }
            break;
        case AST_ASSIGNMENT:
            node->data.assignment.value = propagate_node(node->data.assignment.value, bindings);
            if (is_constant_literal(node->data.assignment.value)) {
                set_binding(bindings, node->data.assignment.name, node->data.assignment.value);
            } else {
                remove_binding(bindings, node->data.assignment.name);
            }
            break;
        case AST_IF_STMT:
            return propagate_if(node, bindings);
        case AST_WHILE_STMT:
            return propagate_while(node, bindings);
        case AST_RETURN_STMT:
            node->data.return_stmt.value = propagate_node(node->data.return_stmt.value, bindings);
            break;
        case AST_ARDUINO_CALL:
            node->data.arduino_call.first_arg = propagate_node(node->data.arduino_call.first_arg, bindings);
            node->data.arduino_call.second_arg = propagate_node(node->data.arduino_call.second_arg, bindings);
            break;
        case AST_BINARY_EXPR:
            node->data.binary_expr.left = propagate_node(node->data.binary_expr.left, bindings);
            node->data.binary_expr.right = propagate_node(node->data.binary_expr.right, bindings);
            break;
        case AST_UNARY_EXPR:
            node->data.unary_expr.operand = propagate_node(node->data.unary_expr.operand, bindings);
            break;
        case AST_IDENTIFIER:
            return propagate_identifier(node, bindings);
        default:
            break;
    }

    return node;
}

ASTNode *propagate_constants(ASTNode *root) {
    ConstantBinding *bindings = NULL;

    root = propagate_node(root, &bindings);
    free_bindings(bindings);
    return root;
}
