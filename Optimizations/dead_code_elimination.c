#include "dead_code_elimination.h"

#include <stdlib.h>

static ASTNode *eliminate_node(ASTNode *node);

static int literal_truth_value(const ASTNode *node, int *truth_out) {
    long number_value;
    char *end;

    if (node == NULL || truth_out == NULL) {
        return 0;
    }

    if (node->kind == AST_BOOL_LITERAL) {
        *truth_out = node->data.literal.bool_value ? 1 : 0;
        return 1;
    }

    if (node->kind != AST_NUMBER_LITERAL) {
        return 0;
    }

    number_value = strtol(node->data.literal.value, &end, 10);
    if (end == NULL || *end != '\0') {
        return 0;
    }

    *truth_out = (number_value != 0) ? 1 : 0;
    return 1;
}

static int statement_stops_control_flow(const ASTNode *node) {
    return node != NULL
        && (node->kind == AST_RETURN_STMT || node->kind == AST_BREAK_STMT);
}

static void eliminate_list(ASTNodeList **list_ref) {
    ASTNodeList **cursor_ref;
    int unreachable = 0;

    if (list_ref == NULL) {
        return;
    }

    cursor_ref = list_ref;
    while (*cursor_ref != NULL) {
        ASTNodeList *cursor = *cursor_ref;

        if (unreachable) {
            *cursor_ref = cursor->next;
            ast_free(cursor->node);
            free(cursor);
            continue;
        }

        cursor->node = eliminate_node(cursor->node);
        if (cursor->node == NULL) {
            *cursor_ref = cursor->next;
            free(cursor);
            continue;
        }

        if (statement_stops_control_flow(cursor->node)) {
            unreachable = 1;
        }

        cursor_ref = &cursor->next;
    }
}

static ASTNode *eliminate_if(ASTNode *node) {
    int truth_value;
    ASTNode *replacement;
    ASTNode *dead_branch;

    node->data.if_stmt.condition = eliminate_node(node->data.if_stmt.condition);
    node->data.if_stmt.then_branch = eliminate_node(node->data.if_stmt.then_branch);
    node->data.if_stmt.else_branch = eliminate_node(node->data.if_stmt.else_branch);

    if (!literal_truth_value(node->data.if_stmt.condition, &truth_value)) {
        return node;
    }

    replacement = truth_value ? node->data.if_stmt.then_branch : node->data.if_stmt.else_branch;
    dead_branch = truth_value ? node->data.if_stmt.else_branch : node->data.if_stmt.then_branch;
    ast_free(dead_branch);
    node->data.if_stmt.then_branch = NULL;
    node->data.if_stmt.else_branch = NULL;
    ast_free(node);
    return replacement;
}

static ASTNode *eliminate_while(ASTNode *node) {
    int truth_value;

    node->data.while_stmt.condition = eliminate_node(node->data.while_stmt.condition);
    node->data.while_stmt.body = eliminate_node(node->data.while_stmt.body);

    if (literal_truth_value(node->data.while_stmt.condition, &truth_value) && !truth_value) {
        ast_free(node);
        return NULL;
    }

    return node;
}

static ASTNode *eliminate_node(ASTNode *node) {
    if (node == NULL) {
        return NULL;
    }

    switch (node->kind) {
        case AST_PROGRAM:
            eliminate_list(&node->data.program.functions);
            break;
        case AST_FUNCTION:
            node->data.function.body = eliminate_node(node->data.function.body);
            break;
        case AST_BLOCK:
            eliminate_list(&node->data.block.statements);
            break;
        case AST_DECLARATION:
            node->data.declaration.initializer = eliminate_node(node->data.declaration.initializer);
            break;
        case AST_ASSIGNMENT:
            node->data.assignment.value = eliminate_node(node->data.assignment.value);
            break;
        case AST_IF_STMT:
            return eliminate_if(node);
        case AST_WHILE_STMT:
            return eliminate_while(node);
        case AST_RETURN_STMT:
            node->data.return_stmt.value = eliminate_node(node->data.return_stmt.value);
            break;
        case AST_ARDUINO_CALL:
            node->data.arduino_call.first_arg = eliminate_node(node->data.arduino_call.first_arg);
            node->data.arduino_call.second_arg = eliminate_node(node->data.arduino_call.second_arg);
            break;
        case AST_BINARY_EXPR:
            node->data.binary_expr.left = eliminate_node(node->data.binary_expr.left);
            node->data.binary_expr.right = eliminate_node(node->data.binary_expr.right);
            break;
        case AST_UNARY_EXPR:
            node->data.unary_expr.operand = eliminate_node(node->data.unary_expr.operand);
            break;
        default:
            break;
    }

    return node;
}

ASTNode *eliminate_dead_code(ASTNode *root) {
    return eliminate_node(root);
}
