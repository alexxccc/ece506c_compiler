#include "constant_folding.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ASTNode *fold_node(ASTNode *node);
static void fold_list(ASTNodeList *list);

static ASTNode *make_number_result(long value, int line) {
    char buffer[32];

    snprintf(buffer, sizeof(buffer), "%ld", value);
    return ast_make_number_literal(buffer, line);
}

static ASTNode *make_bool_result(int value, int line) {
    return ast_make_bool_literal(value ? 1 : 0, line);
}

static int is_number_literal(const ASTNode *node) {
    return node != NULL && node->kind == AST_NUMBER_LITERAL;
}

static int is_bool_literal(const ASTNode *node) {
    return node != NULL && node->kind == AST_BOOL_LITERAL;
}

static int parse_number_literal(const ASTNode *node, long *value_out) {
    char *end;
    long value;

    if (!is_number_literal(node) || value_out == NULL) {
        return 0;
    }

    value = strtol(node->data.literal.value, &end, 10);
    if (end == NULL || *end != '\0') {
        return 0;
    }

    *value_out = value;
    return 1;
}

static int parse_truthy_literal(const ASTNode *node, int *value_out) {
    long number_value;

    if (value_out == NULL || node == NULL) {
        return 0;
    }

    if (is_bool_literal(node)) {
        *value_out = node->data.literal.bool_value ? 1 : 0;
        return 1;
    }

    if (parse_number_literal(node, &number_value)) {
        *value_out = (number_value != 0) ? 1 : 0;
        return 1;
    }

    return 0;
}

static ASTNode *fold_unary_expression(ASTNode *node) {
    int truth_value;
    ASTNode *replacement;

    if (node == NULL || node->kind != AST_UNARY_EXPR) {
        return node;
    }

    node->data.unary_expr.operand = fold_node(node->data.unary_expr.operand);

    if (!parse_truthy_literal(node->data.unary_expr.operand, &truth_value)) {
        return node;
    }

    replacement = make_bool_result(!truth_value, node->line);
    replacement->inferred_type = TYPE_BOOL;
    ast_free(node);
    return replacement;
}

static int fold_binary_to_literal(ASTNode *node, ASTNode **replacement_out) {
    long left_number;
    long right_number;
    int left_truth;
    int right_truth;
    ASTNode *replacement = NULL;

    if (node == NULL || replacement_out == NULL || node->kind != AST_BINARY_EXPR) {
        return 0;
    }

    switch (node->data.binary_expr.op) {
        case OP_ADD:
            if (parse_number_literal(node->data.binary_expr.left, &left_number)
                && parse_number_literal(node->data.binary_expr.right, &right_number)) {
                replacement = make_number_result(left_number + right_number, node->line);
                replacement->inferred_type = TYPE_INT;
            }
            break;
        case OP_SUB:
            if (parse_number_literal(node->data.binary_expr.left, &left_number)
                && parse_number_literal(node->data.binary_expr.right, &right_number)) {
                replacement = make_number_result(left_number - right_number, node->line);
                replacement->inferred_type = TYPE_INT;
            }
            break;
        case OP_MUL:
            if (parse_number_literal(node->data.binary_expr.left, &left_number)
                && parse_number_literal(node->data.binary_expr.right, &right_number)) {
                replacement = make_number_result(left_number * right_number, node->line);
                replacement->inferred_type = TYPE_INT;
            }
            break;
        case OP_DIV:
            if (parse_number_literal(node->data.binary_expr.left, &left_number)
                && parse_number_literal(node->data.binary_expr.right, &right_number)
                && right_number != 0) {
                replacement = make_number_result(left_number / right_number, node->line);
                replacement->inferred_type = TYPE_INT;
            }
            break;
        case OP_MOD:
            if (parse_number_literal(node->data.binary_expr.left, &left_number)
                && parse_number_literal(node->data.binary_expr.right, &right_number)
                && right_number != 0) {
                replacement = make_number_result(left_number % right_number, node->line);
                replacement->inferred_type = TYPE_INT;
            }
            break;
        case OP_EQ:
            if (parse_number_literal(node->data.binary_expr.left, &left_number)
                && parse_number_literal(node->data.binary_expr.right, &right_number)) {
                replacement = make_bool_result(left_number == right_number, node->line);
            } else if (parse_truthy_literal(node->data.binary_expr.left, &left_truth)
                       && parse_truthy_literal(node->data.binary_expr.right, &right_truth)) {
                replacement = make_bool_result(left_truth == right_truth, node->line);
            }
            break;
        case OP_NEQ:
            if (parse_number_literal(node->data.binary_expr.left, &left_number)
                && parse_number_literal(node->data.binary_expr.right, &right_number)) {
                replacement = make_bool_result(left_number != right_number, node->line);
            } else if (parse_truthy_literal(node->data.binary_expr.left, &left_truth)
                       && parse_truthy_literal(node->data.binary_expr.right, &right_truth)) {
                replacement = make_bool_result(left_truth != right_truth, node->line);
            }
            break;
        case OP_LT:
            if (parse_number_literal(node->data.binary_expr.left, &left_number)
                && parse_number_literal(node->data.binary_expr.right, &right_number)) {
                replacement = make_bool_result(left_number < right_number, node->line);
            }
            break;
        case OP_LTE:
            if (parse_number_literal(node->data.binary_expr.left, &left_number)
                && parse_number_literal(node->data.binary_expr.right, &right_number)) {
                replacement = make_bool_result(left_number <= right_number, node->line);
            }
            break;
        case OP_GT:
            if (parse_number_literal(node->data.binary_expr.left, &left_number)
                && parse_number_literal(node->data.binary_expr.right, &right_number)) {
                replacement = make_bool_result(left_number > right_number, node->line);
            }
            break;
        case OP_GTE:
            if (parse_number_literal(node->data.binary_expr.left, &left_number)
                && parse_number_literal(node->data.binary_expr.right, &right_number)) {
                replacement = make_bool_result(left_number >= right_number, node->line);
            }
            break;
        case OP_AND:
            if (parse_truthy_literal(node->data.binary_expr.left, &left_truth)
                && parse_truthy_literal(node->data.binary_expr.right, &right_truth)) {
                replacement = make_bool_result(left_truth && right_truth, node->line);
            }
            break;
        case OP_OR:
            if (parse_truthy_literal(node->data.binary_expr.left, &left_truth)
                && parse_truthy_literal(node->data.binary_expr.right, &right_truth)) {
                replacement = make_bool_result(left_truth || right_truth, node->line);
            }
            break;
        default:
            break;
    }

    if (replacement == NULL) {
        return 0;
    }

    if (replacement->kind == AST_BOOL_LITERAL) {
        replacement->inferred_type = TYPE_BOOL;
    }

    *replacement_out = replacement;
    return 1;
}

static ASTNode *fold_binary_expression(ASTNode *node) {
    ASTNode *replacement;

    if (node == NULL || node->kind != AST_BINARY_EXPR) {
        return node;
    }

    node->data.binary_expr.left = fold_node(node->data.binary_expr.left);
    node->data.binary_expr.right = fold_node(node->data.binary_expr.right);

    if (!fold_binary_to_literal(node, &replacement)) {
        return node;
    }

    ast_free(node);
    return replacement;
}

static void fold_list(ASTNodeList *list) {
    ASTNodeList *cursor = list;

    while (cursor != NULL) {
        cursor->node = fold_node(cursor->node);
        cursor = cursor->next;
    }
}

static ASTNode *fold_node(ASTNode *node) {
    if (node == NULL) {
        return NULL;
    }

    switch (node->kind) {
        case AST_PROGRAM:
            fold_list(node->data.program.includes);
            fold_list(node->data.program.functions);
            break;
        case AST_FUNCTION:
            node->data.function.body = fold_node(node->data.function.body);
            break;
        case AST_BLOCK:
            fold_list(node->data.block.statements);
            break;
        case AST_DECLARATION:
            node->data.declaration.initializer = fold_node(node->data.declaration.initializer);
            break;
        case AST_ASSIGNMENT:
            node->data.assignment.value = fold_node(node->data.assignment.value);
            break;
        case AST_IF_STMT:
            node->data.if_stmt.condition = fold_node(node->data.if_stmt.condition);
            node->data.if_stmt.then_branch = fold_node(node->data.if_stmt.then_branch);
            node->data.if_stmt.else_branch = fold_node(node->data.if_stmt.else_branch);
            break;
        case AST_WHILE_STMT:
            node->data.while_stmt.condition = fold_node(node->data.while_stmt.condition);
            node->data.while_stmt.body = fold_node(node->data.while_stmt.body);
            break;
        case AST_RETURN_STMT:
            node->data.return_stmt.value = fold_node(node->data.return_stmt.value);
            break;
        case AST_ARDUINO_CALL:
            node->data.arduino_call.first_arg = fold_node(node->data.arduino_call.first_arg);
            node->data.arduino_call.second_arg = fold_node(node->data.arduino_call.second_arg);
            break;
        case AST_UNARY_EXPR:
            return fold_unary_expression(node);
        case AST_BINARY_EXPR:
            return fold_binary_expression(node);
        default:
            break;
    }

    return node;
}

ASTNode *fold_constants(ASTNode *root) {
    return fold_node(root);
}
