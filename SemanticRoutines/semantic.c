#include "semantic.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void *xmalloc(size_t size) {
    void *memory = malloc(size);
    if (memory == NULL) {
        fprintf(stderr, "Out of memory while building symbol tables.\n");
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

static Scope *create_scope(SymbolTable *table, Scope *parent, const char *label) {
    // Scopes form a parent chain so inner blocks can see outer names.
    Scope *scope = (Scope *)xmalloc(sizeof(Scope));
    memset(scope, 0, sizeof(Scope));
    scope->id = table->next_scope_id++;
    scope->depth = (parent == NULL) ? 0 : parent->depth + 1;
    scope->label = copy_string(label);
    scope->parent = parent;

    if (table->scope_list_head == NULL) {
        table->scope_list_head = scope;
        table->scope_list_tail = scope;
    } else {
        table->scope_list_tail->next = scope;
        table->scope_list_tail = scope;
    }

    return scope;
}

static SymbolTable *create_symbol_table(void) {
    SymbolTable *table = (SymbolTable *)xmalloc(sizeof(SymbolTable));
    memset(table, 0, sizeof(SymbolTable));
    table->global_scope = create_scope(table, NULL, "global");
    table->current_scope = table->global_scope;
    return table;
}

static void enter_scope(SymbolTable *table, const char *label) {
    table->current_scope = create_scope(table, table->current_scope, label);
}

static void leave_scope(SymbolTable *table) {
    if (table->current_scope != NULL && table->current_scope->parent != NULL) {
        table->current_scope = table->current_scope->parent;
    }
}

static void append_symbol(Scope *scope, Symbol *symbol) {
    Symbol *cursor;

    if (scope->symbols == NULL) {
        scope->symbols = symbol;
        return;
    }

    cursor = scope->symbols;
    while (cursor->next != NULL) {
        cursor = cursor->next;
    }
    cursor->next = symbol;
}

static Symbol *lookup_in_scope(const Scope *scope, const char *name) {
    Symbol *cursor = (scope == NULL) ? NULL : scope->symbols;

    while (cursor != NULL) {
        if (strcmp(cursor->name, name) == 0) {
            return cursor;
        }
        cursor = cursor->next;
    }

    return NULL;
}

static Symbol *lookup_visible_symbol(const SymbolTable *table, const char *name) {
    // Name lookup starts local then moves outward scope by scope
    Scope *scope = table->current_scope;

    while (scope != NULL) {
        Symbol *match = lookup_in_scope(scope, name);
        if (match != NULL) {
            return match;
        }
        scope = scope->parent;
    }

    return NULL;
}

static Symbol *define_symbol(SymbolTable *table, const char *name, SymbolKind kind, TypeKind type, int line) {
    Symbol *symbol = (Symbol *)xmalloc(sizeof(Symbol));
    memset(symbol, 0, sizeof(Symbol));
    symbol->name = copy_string(name);
    symbol->kind = kind;
    symbol->type = type;
    symbol->line = line;
    append_symbol(table->current_scope, symbol);
    return symbol;
}

static void add_error(SymbolTable *table, int line, const char *format, ...) {
    SemanticError *error = (SemanticError *)xmalloc(sizeof(SemanticError));
    char message_buffer[512];
    va_list args;

    memset(error, 0, sizeof(SemanticError));

    va_start(args, format);
    vsnprintf(message_buffer, sizeof(message_buffer), format, args);
    va_end(args);

    error->line = line;
    error->message = copy_string(message_buffer);

    if (table->errors_head == NULL) {
        table->errors_head = error;
        table->errors_tail = error;
    } else {
        table->errors_tail->next = error;
        table->errors_tail = error;
    }

    table->error_count += 1;
}

static int is_same_type(TypeKind left, TypeKind right) {
    return left == right;
}

static int require_type(SymbolTable *table, int line, const char *context, TypeKind expected, TypeKind actual) {
    // Type checking for assignments, conditions etc.
    if (actual == TYPE_ERROR || actual == TYPE_UNKNOWN) {
        return 0;
    }

    if (!is_same_type(expected, actual)) {
        add_error(
            table,
            line,
            "%s expects %s but found %s.",
            context,
            type_kind_name(expected),
            type_kind_name(actual)
        );
        return 1;
    }

    return 0;
}

static TypeKind analyze_expression(SymbolTable *table, ASTNode *node);
static void analyze_statement(SymbolTable *table, ASTNode *node, int loop_depth);

static void analyze_block(SymbolTable *table, ASTNode *node, int loop_depth, int create_new_scope, const char *label) {
    ASTNodeList *cursor;

    if (node == NULL || node->kind != AST_BLOCK) {
        return;
    }

    if (create_new_scope) {
        enter_scope(table, label);
    }

    cursor = node->data.block.statements;
    while (cursor != NULL) {
        analyze_statement(table, cursor->node, loop_depth);
        cursor = cursor->next;
    }

    if (create_new_scope) {
        leave_scope(table);
    }
}

static TypeKind analyze_arduino_call(SymbolTable *table, ASTNode *node) {
    TypeKind first_type = analyze_expression(table, node->data.arduino_call.first_arg);
    TypeKind second_type = analyze_expression(table, node->data.arduino_call.second_arg);

    switch (node->data.arduino_call.call_kind) {
        case CALL_SERIAL_PRINT:
        case CALL_SERIAL_PRINTLN:
            if (first_type == TYPE_VOID) {
                add_error(table, node->line, "%s cannot print a void expression.", arduino_call_name(node->data.arduino_call.call_kind));
            }
            node->inferred_type = TYPE_VOID;
            return TYPE_VOID;
        case CALL_DELAY:
            require_type(table, node->line, "delay", TYPE_INT, first_type);
            node->inferred_type = TYPE_VOID;
            return TYPE_VOID;
        case CALL_PINMODE:
            require_type(table, node->line, "pinMode first argument", TYPE_INT, first_type);
            require_type(table, node->line, "pinMode second argument", TYPE_INT, second_type);
            node->inferred_type = TYPE_VOID;
            return TYPE_VOID;
        case CALL_DIGITAL_WRITE:
            require_type(table, node->line, "digitalWrite first argument", TYPE_INT, first_type);
            require_type(table, node->line, "digitalWrite second argument", TYPE_INT, second_type);
            node->inferred_type = TYPE_VOID;
            return TYPE_VOID;
        case CALL_DIGITAL_READ:
            require_type(table, node->line, "digitalRead argument", TYPE_INT, first_type);
            node->inferred_type = TYPE_INT;
            return TYPE_INT;
        case CALL_ANALOG_WRITE:
            require_type(table, node->line, "analogWrite first argument", TYPE_INT, first_type);
            require_type(table, node->line, "analogWrite second argument", TYPE_INT, second_type);
            node->inferred_type = TYPE_VOID;
            return TYPE_VOID;
        case CALL_ANALOG_READ:
            require_type(table, node->line, "analogRead argument", TYPE_INT, first_type);
            node->inferred_type = TYPE_INT;
            return TYPE_INT;
        default:
            node->inferred_type = TYPE_ERROR;
            return TYPE_ERROR;
    }
}

static TypeKind analyze_binary_expression(SymbolTable *table, ASTNode *node) {
    TypeKind left_type = analyze_expression(table, node->data.binary_expr.left);
    TypeKind right_type = analyze_expression(table, node->data.binary_expr.right);

    if (left_type == TYPE_ERROR || right_type == TYPE_ERROR) {
        node->inferred_type = TYPE_ERROR;
        return TYPE_ERROR;
    }

    switch (node->data.binary_expr.op) {
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_MOD:
            require_type(table, node->line, "arithmetic expression left operand", TYPE_INT, left_type);
            require_type(table, node->line, "arithmetic expression right operand", TYPE_INT, right_type);
            node->inferred_type = TYPE_INT;
            return TYPE_INT;
        case OP_LT:
        case OP_LTE:
        case OP_GT:
        case OP_GTE:
            require_type(table, node->line, "comparison left operand", TYPE_INT, left_type);
            require_type(table, node->line, "comparison right operand", TYPE_INT, right_type);
            node->inferred_type = TYPE_BOOL;
            return TYPE_BOOL;
        case OP_AND:
        case OP_OR:
            require_type(table, node->line, "logical expression left operand", TYPE_BOOL, left_type);
            require_type(table, node->line, "logical expression right operand", TYPE_BOOL, right_type);
            node->inferred_type = TYPE_BOOL;
            return TYPE_BOOL;
        case OP_EQ:
        case OP_NEQ:
            if (!is_same_type(left_type, right_type)) {
                add_error(
                    table,
                    node->line,
                    "Equality comparison expects matching operand types but found %s and %s.",
                    type_kind_name(left_type),
                    type_kind_name(right_type)
                );
            }
            node->inferred_type = TYPE_BOOL;
            return TYPE_BOOL;
        default:
            node->inferred_type = TYPE_ERROR;
            return TYPE_ERROR;
    }
}

static TypeKind analyze_expression(SymbolTable *table, ASTNode *node) {
    // Expressions return a type and store it back on the AST node.
    Symbol *symbol;

    if (node == NULL) {
        return TYPE_VOID;
    }

    switch (node->kind) {
        case AST_IDENTIFIER:
            symbol = lookup_visible_symbol(table, node->data.identifier.name);
            if (symbol == NULL) {
                add_error(table, node->line, "Identifier '%s' is not declared in this scope.", node->data.identifier.name);
                node->inferred_type = TYPE_ERROR;
                return TYPE_ERROR;
            }
            node->inferred_type = symbol->type;
            return symbol->type;
        case AST_NUMBER_LITERAL:
            node->inferred_type = TYPE_INT;
            return TYPE_INT;
        case AST_STRING_LITERAL:
            node->inferred_type = TYPE_STRING;
            return TYPE_STRING;
        case AST_BOOL_LITERAL:
            node->inferred_type = TYPE_BOOL;
            return TYPE_BOOL;
        case AST_UNARY_EXPR:
            require_type(
                table,
                node->line,
                "logical negation operand",
                TYPE_BOOL,
                analyze_expression(table, node->data.unary_expr.operand)
            );
            node->inferred_type = TYPE_BOOL;
            return TYPE_BOOL;
        case AST_BINARY_EXPR:
            return analyze_binary_expression(table, node);
        case AST_ARDUINO_CALL:
            return analyze_arduino_call(table, node);
        default:
            node->inferred_type = TYPE_ERROR;
            return TYPE_ERROR;
    }
}

static void analyze_declaration(SymbolTable *table, ASTNode *node) {
    Symbol *existing;
    TypeKind initializer_type;

    existing = lookup_in_scope(table->current_scope, node->data.declaration.name);
    if (existing != NULL) {
        add_error(table, node->line, "Redeclaration of '%s' in the same scope.", node->data.declaration.name);
        return;
    }

    define_symbol(table, node->data.declaration.name, SYMBOL_VARIABLE, node->data.declaration.declared_type, node->line);

    if (node->data.declaration.initializer != NULL) {
        initializer_type = analyze_expression(table, node->data.declaration.initializer);
        if (initializer_type != TYPE_ERROR && !is_same_type(node->data.declaration.declared_type, initializer_type)) {
            add_error(
                table,
                node->line,
                "Cannot initialize '%s' of type %s with a value of type %s.",
                node->data.declaration.name,
                type_kind_name(node->data.declaration.declared_type),
                type_kind_name(initializer_type)
            );
        }
    }
}

static void analyze_assignment(SymbolTable *table, ASTNode *node) {
    Symbol *symbol = lookup_visible_symbol(table, node->data.assignment.name);
    TypeKind value_type = analyze_expression(table, node->data.assignment.value);

    if (symbol == NULL) {
        add_error(table, node->line, "Assignment target '%s' is not declared in this scope.", node->data.assignment.name);
        node->inferred_type = TYPE_ERROR;
        return;
    }

    node->inferred_type = symbol->type;

    if (value_type != TYPE_ERROR && !is_same_type(symbol->type, value_type)) {
        add_error(
            table,
            node->line,
            "Cannot assign a value of type %s to '%s' of type %s.",
            type_kind_name(value_type),
            node->data.assignment.name,
            type_kind_name(symbol->type)
        );
    }
}

static void analyze_statement(SymbolTable *table, ASTNode *node, int loop_depth) {
    // Statements drive scope changes and decide which expressions to check
    TypeKind condition_type;

    if (node == NULL) {
        return;
    }

    switch (node->kind) {
        case AST_BLOCK:
            analyze_block(table, node, loop_depth, 1, "block");
            break;
        case AST_DECLARATION:
            analyze_declaration(table, node);
            break;
        case AST_ASSIGNMENT:
            analyze_assignment(table, node);
            break;
        case AST_IF_STMT:
            condition_type = analyze_expression(table, node->data.if_stmt.condition);
            require_type(table, node->line, "if condition", TYPE_BOOL, condition_type);
            analyze_statement(table, node->data.if_stmt.then_branch, loop_depth);
            analyze_statement(table, node->data.if_stmt.else_branch, loop_depth);
            break;
        case AST_WHILE_STMT:
            condition_type = analyze_expression(table, node->data.while_stmt.condition);
            require_type(table, node->line, "while condition", TYPE_BOOL, condition_type);
            analyze_statement(table, node->data.while_stmt.body, loop_depth + 1);
            break;
        case AST_RETURN_STMT:
            if (node->data.return_stmt.value != NULL) {
                add_error(table, node->line, "Void functions cannot return a value.");
                (void)analyze_expression(table, node->data.return_stmt.value);
            }
            break;
        case AST_BREAK_STMT:
            if (loop_depth == 0) {
                add_error(table, node->line, "break can only appear inside a while loop.");
            }
            break;
        case AST_ARDUINO_CALL:
            (void)analyze_expression(table, node);
            break;
        default:
            break;
    }
}

static void declare_functions(SymbolTable *table, ASTNodeList *functions) {
    ASTNodeList *cursor = functions;

    while (cursor != NULL) {
        ASTNode *function = cursor->node;
        if (function != NULL && function->kind == AST_FUNCTION) {
            if (lookup_in_scope(table->global_scope, function->data.function.name) != NULL) {
                add_error(table, function->line, "Function '%s' is declared more than once.", function->data.function.name);
            } else {
                define_symbol(table, function->data.function.name, SYMBOL_FUNCTION, function->data.function.return_type, function->line);
            }
        }
        cursor = cursor->next;
    }
}

static void analyze_functions(SymbolTable *table, ASTNodeList *functions) {
    ASTNodeList *cursor = functions;

    while (cursor != NULL) {
        ASTNode *function = cursor->node;
        if (function != NULL && function->kind == AST_FUNCTION) {
            enter_scope(table, function->data.function.name);
            analyze_block(table, function->data.function.body, 0, 0, function->data.function.name);
            leave_scope(table);
        }
        cursor = cursor->next;
    }
}

SymbolTable *analyze_program(ASTNode *program) {
    // Main semantic pass: declare functions first, then analyze bodies
    SymbolTable *table = create_symbol_table();

    if (program == NULL || program->kind != AST_PROGRAM) {
        add_error(table, 0, "No program AST was produced by the parser.");
        return table;
    }

    declare_functions(table, program->data.program.functions);
    analyze_functions(table, program->data.program.functions);
    return table;
}

const char *symbol_kind_name(SymbolKind kind) {
    switch (kind) {
        case SYMBOL_FUNCTION:
            return "function";
        case SYMBOL_VARIABLE:
        default:
            return "variable";
    }
}

void print_symbol_tables(const SymbolTable *table, FILE *out) {
    const Scope *scope = table == NULL ? NULL : table->scope_list_head;

    if (table == NULL) {
        fprintf(out, "No symbol table available.\n");
        return;
    }

    fprintf(out, "Symbol Tables\n");
    while (scope != NULL) {
        const Symbol *symbol = scope->symbols;
        fprintf(out, "Scope #%d (depth=%d, label=%s)\n", scope->id, scope->depth, scope->label);
        if (symbol == NULL) {
            fprintf(out, "  <empty>\n");
        }
        while (symbol != NULL) {
            fprintf(out, "  %s : %s, type=%s, declared_at_line=%d\n", symbol->name, symbol_kind_name(symbol->kind), type_kind_name(symbol->type), symbol->line);
            symbol = symbol->next;
        }
        scope = scope->next;
    }
}

void print_semantic_errors(const SymbolTable *table, FILE *out) {
    const SemanticError *error = table == NULL ? NULL : table->errors_head;

    if (table == NULL || table->error_count == 0) {
        fprintf(out, "Semantic Errors: none\n");
        return;
    }

    fprintf(out, "Semantic Errors (%d)\n", table->error_count);
    while (error != NULL) {
        fprintf(out, "  line %d: %s\n", error->line, error->message);
        error = error->next;
    }
}

int semantic_error_count(const SymbolTable *table) {
    return table == NULL ? 0 : table->error_count;
}

void free_symbol_table(SymbolTable *table) {
    Scope *scope;
    SemanticError *error;

    if (table == NULL) {
        return;
    }

    scope = table->scope_list_head;
    while (scope != NULL) {
        Scope *next_scope = scope->next;
        Symbol *symbol = scope->symbols;

        while (symbol != NULL) {
            Symbol *next_symbol = symbol->next;
            free(symbol->name);
            free(symbol);
            symbol = next_symbol;
        }

        free(scope->label);
        free(scope);
        scope = next_scope;
    }

    error = table->errors_head;
    while (error != NULL) {
        SemanticError *next_error = error->next;
        free(error->message);
        free(error);
        error = next_error;
    }

    free(table);
}
