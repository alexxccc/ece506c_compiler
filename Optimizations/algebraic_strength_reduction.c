#include "algebraic_strength_reduction.h"

#include <string.h>

static ASTNode *reduce_node(ASTNode *node);

static int is_number_value(const ASTNode *node, const char *value) {
    return node != NULL
        && node->kind == AST_NUMBER_LITERAL
        && node->data.literal.value != NULL
        && strcmp(node->data.literal.value, value) == 0;
}

static ASTNode *make_zero_literal(int line) {
    return ast_make_number_literal("0", line);
}

static ASTNode *take_left_child(ASTNode *node) {
    ASTNode *replacement = node->data.binary_expr.left;

    node->data.binary_expr.left = NULL;
    ast_free(node);
    return replacement;
}

static ASTNode *take_right_child(ASTNode *node) {
    ASTNode *replacement = node->data.binary_expr.right;

    node->data.binary_expr.right = NULL;
    ast_free(node);
    return replacement;
}

static ASTNode *replace_with_zero(ASTNode *node) {
    ASTNode *replacement = make_zero_literal(node->line);

    ast_free(node);
    return replacement;
}

static ASTNode *reduce_binary_expression(ASTNode *node) {
    if (node == NULL || node->kind != AST_BINARY_EXPR) {
        return node;
    }

    node->data.binary_expr.left = reduce_node(node->data.binary_expr.left);
    node->data.binary_expr.right = reduce_node(node->data.binary_expr.right);

    switch (node->data.binary_expr.op) {
        case OP_ADD:
            // Reduce add by 0 to the other operand
            if (is_number_value(node->data.binary_expr.left, "0")) {
                return take_right_child(node);
            }
            if (is_number_value(node->data.binary_expr.right, "0")) {
                return take_left_child(node);
            }
            break;
        case OP_SUB:
            // Reduce sub by 0 to the left operand
            if (is_number_value(node->data.binary_expr.right, "0")) {
                return take_left_child(node);
            }
            break;
        case OP_MUL:
            // Reduce mult by 0 to 0, and mult by 1 to the other operand
            if (is_number_value(node->data.binary_expr.left, "0")
                || is_number_value(node->data.binary_expr.right, "0")) {
                return replace_with_zero(node);
            }
            if (is_number_value(node->data.binary_expr.left, "1")) {
                return take_right_child(node);
            }
            if (is_number_value(node->data.binary_expr.right, "1")) {
                return take_left_child(node);
            }
            break;
        case OP_DIV:
            // Reduce div by 1 to the other operand
            if (is_number_value(node->data.binary_expr.right, "1")) {
                return take_left_child(node);
            }
            break;
        default:
            break;
    }

    return node;
}

static void reduce_list(ASTNodeList *list) {
    while (list != NULL) {
        list->node = reduce_node(list->node);
        list = list->next;
    }
}

static ASTNode *reduce_node(ASTNode *node) {
    // Find all places where you could reduce and reduce
    if (node == NULL) {
        return NULL;
    }

    switch (node->kind) {
        case AST_PROGRAM:
            reduce_list(node->data.program.includes);
            reduce_list(node->data.program.functions);
            break;
        case AST_FUNCTION:
            node->data.function.body = reduce_node(node->data.function.body);
            break;
        case AST_BLOCK:
            reduce_list(node->data.block.statements);
            break;
        case AST_DECLARATION:
            node->data.declaration.initializer = reduce_node(node->data.declaration.initializer);
            break;
        case AST_ASSIGNMENT:
            node->data.assignment.value = reduce_node(node->data.assignment.value);
            break;
        case AST_IF_STMT:
            node->data.if_stmt.condition = reduce_node(node->data.if_stmt.condition);
            node->data.if_stmt.then_branch = reduce_node(node->data.if_stmt.then_branch);
            node->data.if_stmt.else_branch = reduce_node(node->data.if_stmt.else_branch);
            break;
        case AST_WHILE_STMT:
            node->data.while_stmt.condition = reduce_node(node->data.while_stmt.condition);
            node->data.while_stmt.body = reduce_node(node->data.while_stmt.body);
            break;
        case AST_RETURN_STMT:
            node->data.return_stmt.value = reduce_node(node->data.return_stmt.value);
            break;
        case AST_ARDUINO_CALL:
            node->data.arduino_call.first_arg = reduce_node(node->data.arduino_call.first_arg);
            node->data.arduino_call.second_arg = reduce_node(node->data.arduino_call.second_arg);
            break;
        case AST_BINARY_EXPR:
            return reduce_binary_expression(node);
        case AST_UNARY_EXPR:
            node->data.unary_expr.operand = reduce_node(node->data.unary_expr.operand);
            break;
        default:
            break;
    }

    return node;
}

ASTNode *reduce_algebraic_strength(ASTNode *root) {
    return reduce_node(root);
}
