#ifndef AST_H
#define AST_H

#include <stdio.h>

typedef enum {
    TYPE_UNKNOWN = 0,
    TYPE_INT,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_VOID,
    TYPE_ERROR
} TypeKind;

typedef enum {
    AST_PROGRAM = 0,
    AST_INCLUDE,
    AST_FUNCTION,
    AST_BLOCK,
    AST_DECLARATION,
    AST_ASSIGNMENT,
    AST_IF_STMT,
    AST_WHILE_STMT,
    AST_RETURN_STMT,
    AST_BREAK_STMT,
    AST_ARDUINO_CALL,
    AST_IDENTIFIER,
    AST_NUMBER_LITERAL,
    AST_STRING_LITERAL,
    AST_BOOL_LITERAL,
    AST_BINARY_EXPR,
    AST_UNARY_EXPR
} ASTNodeKind;

typedef enum {
    OP_ADD = 0,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_EQ,
    OP_NEQ,
    OP_LT,
    OP_LTE,
    OP_GT,
    OP_GTE,
    OP_AND,
    OP_OR
} BinaryOp;

typedef enum {
    OP_NOT = 0
} UnaryOp;

typedef enum {
    CALL_SERIAL_PRINT = 0,
    CALL_SERIAL_PRINTLN,
    CALL_DELAY,
    CALL_PINMODE,
    CALL_DIGITAL_WRITE,
    CALL_DIGITAL_READ,
    CALL_ANALOG_WRITE,
    CALL_ANALOG_READ
} ArduinoCallKind;

typedef struct ASTNode ASTNode;
typedef struct ASTNodeList ASTNodeList;

struct ASTNodeList {
    ASTNode *node;
    ASTNodeList *next;
};

struct ASTNode {
    ASTNodeKind kind;
    TypeKind inferred_type;
    int line;
    union {
        struct {
            ASTNodeList *includes;
            ASTNodeList *functions;
        } program;
        struct {
            char *name;
        } include_stmt;
        struct {
            char *name;
            TypeKind return_type;
            ASTNode *body;
        } function;
        struct {
            ASTNodeList *statements;
        } block;
        struct {
            TypeKind declared_type;
            char *name;
            ASTNode *initializer;
        } declaration;
        struct {
            char *name;
            ASTNode *value;
        } assignment;
        struct {
            ASTNode *condition;
            ASTNode *then_branch;
            ASTNode *else_branch;
        } if_stmt;
        struct {
            ASTNode *condition;
            ASTNode *body;
        } while_stmt;
        struct {
            ASTNode *value;
        } return_stmt;
        struct {
            ArduinoCallKind call_kind;
            ASTNode *first_arg;
            ASTNode *second_arg;
        } arduino_call;
        struct {
            char *name;
        } identifier;
        struct {
            char *value;
            int bool_value;
        } literal;
        struct {
            BinaryOp op;
            ASTNode *left;
            ASTNode *right;
        } binary_expr;
        struct {
            UnaryOp op;
            ASTNode *operand;
        } unary_expr;
    } data;
};

ASTNodeList *ast_list_append(ASTNodeList *list, ASTNode *node);

ASTNode *ast_make_program(ASTNodeList *includes, ASTNodeList *functions, int line);
ASTNode *ast_make_include(const char *name, int line);
ASTNode *ast_make_function(const char *name, TypeKind return_type, ASTNode *body, int line);
ASTNode *ast_make_block(ASTNodeList *statements, int line);
ASTNode *ast_make_declaration(TypeKind declared_type, const char *name, ASTNode *initializer, int line);
ASTNode *ast_make_assignment(const char *name, ASTNode *value, int line);
ASTNode *ast_make_if(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch, int line);
ASTNode *ast_make_while(ASTNode *condition, ASTNode *body, int line);
ASTNode *ast_make_return(ASTNode *value, int line);
ASTNode *ast_make_break(int line);
ASTNode *ast_make_arduino_call(ArduinoCallKind call_kind, ASTNode *first_arg, ASTNode *second_arg, int line);
ASTNode *ast_make_identifier(const char *name, int line);
ASTNode *ast_make_number_literal(const char *value, int line);
ASTNode *ast_make_string_literal(const char *value, int line);
ASTNode *ast_make_bool_literal(int bool_value, int line);
ASTNode *ast_make_binary(BinaryOp op, ASTNode *left, ASTNode *right, int line);
ASTNode *ast_make_unary(UnaryOp op, ASTNode *operand, int line);

void ast_print_post_order(const ASTNode *node, FILE *out);
void ast_free(ASTNode *node);

const char *type_kind_name(TypeKind kind);
const char *binary_op_name(BinaryOp op);
const char *unary_op_name(UnaryOp op);
const char *arduino_call_name(ArduinoCallKind kind);

#endif
