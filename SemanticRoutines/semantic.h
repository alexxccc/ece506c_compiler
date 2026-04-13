#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdio.h>

#include "ast.h"

typedef enum {
    SYMBOL_VARIABLE = 0,
    SYMBOL_FUNCTION
} SymbolKind;

typedef struct Symbol Symbol;
typedef struct Scope Scope;
typedef struct SemanticError SemanticError;
typedef struct SymbolTable SymbolTable;

struct Symbol {
    char *name;
    SymbolKind kind;
    TypeKind type;
    int line;
    Symbol *next;
};

struct Scope {
    int id;
    int depth;
    char *label;
    Symbol *symbols;
    Scope *parent;
    Scope *next;
};

struct SemanticError {
    int line;
    char *message;
    SemanticError *next;
};

struct SymbolTable {
    Scope *global_scope;
    Scope *current_scope;
    Scope *scope_list_head;
    Scope *scope_list_tail;
    SemanticError *errors_head;
    SemanticError *errors_tail;
    int next_scope_id;
    int error_count;
};

SymbolTable *analyze_program(ASTNode *program);
void print_symbol_tables(const SymbolTable *table, FILE *out);
void print_semantic_errors(const SymbolTable *table, FILE *out);
int semantic_error_count(const SymbolTable *table);
void free_symbol_table(SymbolTable *table);

const char *symbol_kind_name(SymbolKind kind);

#endif
