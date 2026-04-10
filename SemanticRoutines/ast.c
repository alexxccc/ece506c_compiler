#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void *xmalloc(size_t size) {
    void *memory = malloc(size);
    if (memory == NULL) {
        fprintf(stderr, "Out of memory while building the AST.\n");
        exit(EXIT_FAILURE);
    }
    return memory;
}

static char *copy_string(const char *text) {
    size_t length;
    char *copy;

    if (text == NULL) {
        return NULL;
    }

    length = strlen(text);
    copy = (char *)xmalloc(length + 1);
    memcpy(copy, text, length + 1);
    return copy;
}

static ASTNode *make_node(ASTNodeKind kind, int line) {
    ASTNode *node = (ASTNode *)xmalloc(sizeof(ASTNode));
    memset(node, 0, sizeof(ASTNode));
    node->kind = kind;
    node->inferred_type = TYPE_UNKNOWN;
    node->line = line;
    return node;
}

ASTNodeList *ast_list_append(ASTNodeList *list, ASTNode *node) {
    ASTNodeList *entry;
    ASTNodeList *cursor;

    if (node == NULL) {
        return list;
    }

    entry = (ASTNodeList *)xmalloc(sizeof(ASTNodeList));
    entry->node = node;
    entry->next = NULL;

    if (list == NULL) {
        return entry;
    }

    cursor = list;
    while (cursor->next != NULL) {
        cursor = cursor->next;
    }
    cursor->next = entry;
    return list;
}

ASTNode *ast_make_program(ASTNodeList *includes, ASTNodeList *functions, int line) {
    ASTNode *node = make_node(AST_PROGRAM, line);
    node->data.program.includes = includes;
    node->data.program.functions = functions;
    node->inferred_type = TYPE_VOID;
    return node;
}

ASTNode *ast_make_include(const char *name, int line) {
    ASTNode *node = make_node(AST_INCLUDE, line);
    node->data.include_stmt.name = copy_string(name);
    return node;
}

ASTNode *ast_make_function(const char *name, TypeKind return_type, ASTNode *body, int line) {
    ASTNode *node = make_node(AST_FUNCTION, line);
    node->data.function.name = copy_string(name);
    node->data.function.return_type = return_type;
    node->data.function.body = body;
    node->inferred_type = return_type;
    return node;
}

ASTNode *ast_make_block(ASTNodeList *statements, int line) {
    ASTNode *node = make_node(AST_BLOCK, line);
    node->data.block.statements = statements;
    return node;
}

ASTNode *ast_make_declaration(TypeKind declared_type, const char *name, ASTNode *initializer, int line) {
    ASTNode *node = make_node(AST_DECLARATION, line);
    node->data.declaration.declared_type = declared_type;
    node->data.declaration.name = copy_string(name);
    node->data.declaration.initializer = initializer;
    node->inferred_type = declared_type;
    return node;
}

ASTNode *ast_make_assignment(const char *name, ASTNode *value, int line) {
    ASTNode *node = make_node(AST_ASSIGNMENT, line);
    node->data.assignment.name = copy_string(name);
    node->data.assignment.value = value;
    return node;
}

ASTNode *ast_make_if(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch, int line) {
    ASTNode *node = make_node(AST_IF_STMT, line);
    node->data.if_stmt.condition = condition;
    node->data.if_stmt.then_branch = then_branch;
    node->data.if_stmt.else_branch = else_branch;
    return node;
}

ASTNode *ast_make_while(ASTNode *condition, ASTNode *body, int line) {
    ASTNode *node = make_node(AST_WHILE_STMT, line);
    node->data.while_stmt.condition = condition;
    node->data.while_stmt.body = body;
    return node;
}

ASTNode *ast_make_return(ASTNode *value, int line) {
    ASTNode *node = make_node(AST_RETURN_STMT, line);
    node->data.return_stmt.value = value;
    node->inferred_type = TYPE_VOID;
    return node;
}

ASTNode *ast_make_break(int line) {
    return make_node(AST_BREAK_STMT, line);
}

ASTNode *ast_make_arduino_call(ArduinoCallKind call_kind, ASTNode *first_arg, ASTNode *second_arg, int line) {
    ASTNode *node = make_node(AST_ARDUINO_CALL, line);
    node->data.arduino_call.call_kind = call_kind;
    node->data.arduino_call.first_arg = first_arg;
    node->data.arduino_call.second_arg = second_arg;
    return node;
}

ASTNode *ast_make_identifier(const char *name, int line) {
    ASTNode *node = make_node(AST_IDENTIFIER, line);
    node->data.identifier.name = copy_string(name);
    return node;
}

ASTNode *ast_make_number_literal(const char *value, int line) {
    ASTNode *node = make_node(AST_NUMBER_LITERAL, line);
    node->data.literal.value = copy_string(value);
    node->inferred_type = TYPE_INT;
    return node;
}

ASTNode *ast_make_string_literal(const char *value, int line) {
    ASTNode *node = make_node(AST_STRING_LITERAL, line);
    node->data.literal.value = copy_string(value);
    node->inferred_type = TYPE_STRING;
    return node;
}

ASTNode *ast_make_bool_literal(int bool_value, int line) {
    ASTNode *node = make_node(AST_BOOL_LITERAL, line);
    node->data.literal.bool_value = bool_value;
    node->inferred_type = TYPE_BOOL;
    return node;
}

ASTNode *ast_make_binary(BinaryOp op, ASTNode *left, ASTNode *right, int line) {
    ASTNode *node = make_node(AST_BINARY_EXPR, line);
    node->data.binary_expr.op = op;
    node->data.binary_expr.left = left;
    node->data.binary_expr.right = right;
    return node;
}

ASTNode *ast_make_unary(UnaryOp op, ASTNode *operand, int line) {
    ASTNode *node = make_node(AST_UNARY_EXPR, line);
    node->data.unary_expr.op = op;
    node->data.unary_expr.operand = operand;
    return node;
}

const char *type_kind_name(TypeKind kind) {
    switch (kind) {
        case TYPE_INT:
            return "int";
        case TYPE_BOOL:
            return "bool";
        case TYPE_STRING:
            return "string";
        case TYPE_VOID:
            return "void";
        case TYPE_ERROR:
            return "error";
        case TYPE_UNKNOWN:
        default:
            return "unknown";
    }
}

const char *binary_op_name(BinaryOp op) {
    switch (op) {
        case OP_ADD:
            return "+";
        case OP_SUB:
            return "-";
        case OP_MUL:
            return "*";
        case OP_DIV:
            return "/";
        case OP_MOD:
            return "%";
        case OP_EQ:
            return "==";
        case OP_NEQ:
            return "!=";
        case OP_LT:
            return "<";
        case OP_LTE:
            return "<=";
        case OP_GT:
            return ">";
        case OP_GTE:
            return ">=";
        case OP_AND:
            return "&&";
        case OP_OR:
            return "||";
        default:
            return "?";
    }
}

const char *unary_op_name(UnaryOp op) {
    switch (op) {
        case OP_NOT:
        default:
            return "!";
    }
}

const char *arduino_call_name(ArduinoCallKind kind) {
    switch (kind) {
        case CALL_SERIAL_PRINT:
            return "Serial.print";
        case CALL_SERIAL_PRINTLN:
            return "Serial.println";
        case CALL_DELAY:
            return "delay";
        case CALL_PINMODE:
            return "pinMode";
        case CALL_DIGITAL_WRITE:
            return "digitalWrite";
        case CALL_DIGITAL_READ:
            return "digitalRead";
        case CALL_ANALOG_WRITE:
            return "analogWrite";
        case CALL_ANALOG_READ:
            return "analogRead";
        default:
            return "unknownCall";
    }
}

static void print_indent(FILE *out, int indent) {
    int i;
    for (i = 0; i < indent; ++i) {
        fputc(' ', out);
    }
}

static void print_type_suffix(const ASTNode *node, FILE *out) {
    if (node != NULL && node->inferred_type != TYPE_UNKNOWN) {
        fprintf(out, ", type=%s", type_kind_name(node->inferred_type));
    }
}

static void print_list_post_order(const ASTNodeList *list, FILE *out, int indent);

static void print_node_post_order(const ASTNode *node, FILE *out, int indent) {
    if (node == NULL) {
        return;
    }

    switch (node->kind) {
        case AST_PROGRAM:
            print_list_post_order(node->data.program.includes, out, indent + 2);
            print_list_post_order(node->data.program.functions, out, indent + 2);
            print_indent(out, indent);
            fprintf(out, "Program\n");
            break;
        case AST_INCLUDE:
            print_indent(out, indent);
            fprintf(out, "Include(name=%s)\n", node->data.include_stmt.name);
            break;
        case AST_FUNCTION:
            print_node_post_order(node->data.function.body, out, indent + 2);
            print_indent(out, indent);
            fprintf(
                out,
                "Function(name=%s, return=%s)\n",
                node->data.function.name,
                type_kind_name(node->data.function.return_type)
            );
            break;
        case AST_BLOCK:
            print_list_post_order(node->data.block.statements, out, indent + 2);
            print_indent(out, indent);
            fprintf(out, "Block\n");
            break;
        case AST_DECLARATION:
            print_node_post_order(node->data.declaration.initializer, out, indent + 2);
            print_indent(out, indent);
            fprintf(
                out,
                "Declaration(name=%s, declared_type=%s",
                node->data.declaration.name,
                type_kind_name(node->data.declaration.declared_type)
            );
            print_type_suffix(node, out);
            fprintf(out, ")\n");
            break;
        case AST_ASSIGNMENT:
            print_node_post_order(node->data.assignment.value, out, indent + 2);
            print_indent(out, indent);
            fprintf(out, "Assignment(name=%s", node->data.assignment.name);
            print_type_suffix(node, out);
            fprintf(out, ")\n");
            break;
        case AST_IF_STMT:
            print_node_post_order(node->data.if_stmt.condition, out, indent + 2);
            print_node_post_order(node->data.if_stmt.then_branch, out, indent + 2);
            print_node_post_order(node->data.if_stmt.else_branch, out, indent + 2);
            print_indent(out, indent);
            fprintf(out, "IfStatement\n");
            break;
        case AST_WHILE_STMT:
            print_node_post_order(node->data.while_stmt.condition, out, indent + 2);
            print_node_post_order(node->data.while_stmt.body, out, indent + 2);
            print_indent(out, indent);
            fprintf(out, "WhileStatement\n");
            break;
        case AST_RETURN_STMT:
            print_node_post_order(node->data.return_stmt.value, out, indent + 2);
            print_indent(out, indent);
            fprintf(out, "ReturnStatement\n");
            break;
        case AST_BREAK_STMT:
            print_indent(out, indent);
            fprintf(out, "BreakStatement\n");
            break;
        case AST_ARDUINO_CALL:
            print_node_post_order(node->data.arduino_call.first_arg, out, indent + 2);
            print_node_post_order(node->data.arduino_call.second_arg, out, indent + 2);
            print_indent(out, indent);
            fprintf(out, "ArduinoCall(name=%s", arduino_call_name(node->data.arduino_call.call_kind));
            print_type_suffix(node, out);
            fprintf(out, ")\n");
            break;
        case AST_IDENTIFIER:
            print_indent(out, indent);
            fprintf(out, "Identifier(name=%s", node->data.identifier.name);
            print_type_suffix(node, out);
            fprintf(out, ")\n");
            break;
        case AST_NUMBER_LITERAL:
            print_indent(out, indent);
            fprintf(out, "NumberLiteral(value=%s", node->data.literal.value);
            print_type_suffix(node, out);
            fprintf(out, ")\n");
            break;
        case AST_STRING_LITERAL:
            print_indent(out, indent);
            fprintf(out, "StringLiteral(value=%s", node->data.literal.value);
            print_type_suffix(node, out);
            fprintf(out, ")\n");
            break;
        case AST_BOOL_LITERAL:
            print_indent(out, indent);
            fprintf(out, "BooleanLiteral(value=%s", node->data.literal.bool_value ? "true" : "false");
            print_type_suffix(node, out);
            fprintf(out, ")\n");
            break;
        case AST_BINARY_EXPR:
            print_node_post_order(node->data.binary_expr.left, out, indent + 2);
            print_node_post_order(node->data.binary_expr.right, out, indent + 2);
            print_indent(out, indent);
            fprintf(out, "BinaryExpr(op=%s", binary_op_name(node->data.binary_expr.op));
            print_type_suffix(node, out);
            fprintf(out, ")\n");
            break;
        case AST_UNARY_EXPR:
            print_node_post_order(node->data.unary_expr.operand, out, indent + 2);
            print_indent(out, indent);
            fprintf(out, "UnaryExpr(op=%s", unary_op_name(node->data.unary_expr.op));
            print_type_suffix(node, out);
            fprintf(out, ")\n");
            break;
        default:
            print_indent(out, indent);
            fprintf(out, "UnknownNode\n");
            break;
    }
}

static void print_list_post_order(const ASTNodeList *list, FILE *out, int indent) {
    const ASTNodeList *cursor = list;
    while (cursor != NULL) {
        print_node_post_order(cursor->node, out, indent);
        cursor = cursor->next;
    }
}

void ast_print_post_order(const ASTNode *node, FILE *out) {
    if (node == NULL) {
        fprintf(out, "(empty AST)\n");
        return;
    }

    print_node_post_order(node, out, 0);
}

static void free_list(ASTNodeList *list) {
    ASTNodeList *cursor = list;
    while (cursor != NULL) {
        ASTNodeList *next = cursor->next;
        ast_free(cursor->node);
        free(cursor);
        cursor = next;
    }
}

void ast_free(ASTNode *node) {
    if (node == NULL) {
        return;
    }

    switch (node->kind) {
        case AST_PROGRAM:
            free_list(node->data.program.includes);
            free_list(node->data.program.functions);
            break;
        case AST_INCLUDE:
            free(node->data.include_stmt.name);
            break;
        case AST_FUNCTION:
            free(node->data.function.name);
            ast_free(node->data.function.body);
            break;
        case AST_BLOCK:
            free_list(node->data.block.statements);
            break;
        case AST_DECLARATION:
            free(node->data.declaration.name);
            ast_free(node->data.declaration.initializer);
            break;
        case AST_ASSIGNMENT:
            free(node->data.assignment.name);
            ast_free(node->data.assignment.value);
            break;
        case AST_IF_STMT:
            ast_free(node->data.if_stmt.condition);
            ast_free(node->data.if_stmt.then_branch);
            ast_free(node->data.if_stmt.else_branch);
            break;
        case AST_WHILE_STMT:
            ast_free(node->data.while_stmt.condition);
            ast_free(node->data.while_stmt.body);
            break;
        case AST_RETURN_STMT:
            ast_free(node->data.return_stmt.value);
            break;
        case AST_ARDUINO_CALL:
            ast_free(node->data.arduino_call.first_arg);
            ast_free(node->data.arduino_call.second_arg);
            break;
        case AST_IDENTIFIER:
            free(node->data.identifier.name);
            break;
        case AST_NUMBER_LITERAL:
        case AST_STRING_LITERAL:
            free(node->data.literal.value);
            break;
        case AST_BOOL_LITERAL:
            break;
        case AST_BINARY_EXPR:
            ast_free(node->data.binary_expr.left);
            ast_free(node->data.binary_expr.right);
            break;
        case AST_UNARY_EXPR:
            ast_free(node->data.unary_expr.operand);
            break;
        default:
            break;
    }

    free(node);
}
