#include <assert.h>

#include "../optimizer.h"

static ASTNode *make_add_expression(void) {
    return ast_make_binary(
        OP_ADD,
        ast_make_number_literal("2", 1),
        ast_make_number_literal("3", 1),
        1
    );
}

static ASTNode *make_constant_propagation_program(void) {
    ASTNodeList *statements = NULL;
    ASTNodeList *functions = NULL;

    statements = ast_list_append(
        statements,
        ast_make_declaration(TYPE_INT, "known", ast_make_number_literal("7", 1), 1)
    );
    statements = ast_list_append(
        statements,
        ast_make_declaration(TYPE_INT, "copy", ast_make_identifier("known", 2), 2)
    );

    functions = ast_list_append(
        functions,
        ast_make_function("setup", TYPE_VOID, ast_make_block(statements, 1), 1)
    );

    return ast_make_program(NULL, functions, 1);
}

static ASTNode *make_combined_optimization_program(void) {
    ASTNodeList *statements = NULL;
    ASTNodeList *functions = NULL;

    statements = ast_list_append(
        statements,
        ast_make_declaration(TYPE_INT, "known", ast_make_number_literal("2", 1), 1)
    );
    statements = ast_list_append(
        statements,
        ast_make_declaration(
            TYPE_INT,
            "result",
            ast_make_binary(OP_ADD, ast_make_identifier("known", 2), ast_make_number_literal("3", 2), 2),
            2
        )
    );

    functions = ast_list_append(
        functions,
        ast_make_function("setup", TYPE_VOID, ast_make_block(statements, 1), 1)
    );

    return ast_make_program(NULL, functions, 1);
}

static ASTNode *second_statement(ASTNode *program) {
    ASTNode *function = program->data.program.functions->node;
    ASTNode *block = function->data.function.body;
    return block->data.block.statements->next->node;
}

static void test_no_optimizations_leave_tree_unchanged(void) {
    OptimizationOptions options = optimization_options_none();
    ASTNode *root = make_add_expression();
    ASTNode *optimized = optimize_ast(root, &options);

    assert(optimized == root);
    assert(optimized->kind == AST_BINARY_EXPR);

    ast_free(optimized);
}

static void test_constant_folding_can_be_enabled(void) {
    OptimizationOptions options = optimization_options_none();
    ASTNode *root = make_add_expression();
    ASTNode *optimized;

    assert(optimization_enable_by_name(&options, "constant-folding"));
    optimized = optimize_ast(root, &options);

    assert(optimized->kind == AST_NUMBER_LITERAL);
    assert(optimized->data.literal.value != NULL);
    assert(optimized->data.literal.value[0] == '5');
    assert(optimized->data.literal.value[1] == '\0');

    ast_free(optimized);
}

static void test_constant_folding_can_be_disabled(void) {
    OptimizationOptions options = optimization_options_all();
    ASTNode *root = make_add_expression();
    ASTNode *optimized;

    assert(optimization_disable_by_name(&options, "constant-folding"));
    optimized = optimize_ast(root, &options);

    assert(optimized == root);
    assert(optimized->kind == AST_BINARY_EXPR);

    ast_free(optimized);
}

static void test_constant_propagation_can_be_enabled(void) {
    OptimizationOptions options = optimization_options_none();
    ASTNode *root = make_constant_propagation_program();
    ASTNode *copy_declaration;

    assert(optimization_enable_by_name(&options, "constant-propagation"));
    root = optimize_ast(root, &options);
    copy_declaration = second_statement(root);

    assert(copy_declaration->kind == AST_DECLARATION);
    assert(copy_declaration->data.declaration.initializer->kind == AST_NUMBER_LITERAL);
    assert(copy_declaration->data.declaration.initializer->data.literal.value[0] == '7');
    assert(copy_declaration->data.declaration.initializer->data.literal.value[1] == '\0');

    ast_free(root);
}

static void test_constant_propagation_can_be_disabled(void) {
    OptimizationOptions options = optimization_options_all();
    ASTNode *root = make_combined_optimization_program();
    ASTNode *result_declaration;
    ASTNode *initializer;

    assert(optimization_disable_by_name(&options, "constant-propagation"));
    root = optimize_ast(root, &options);
    result_declaration = second_statement(root);
    initializer = result_declaration->data.declaration.initializer;

    assert(result_declaration->kind == AST_DECLARATION);
    assert(initializer->kind == AST_BINARY_EXPR);
    assert(initializer->data.binary_expr.left->kind == AST_IDENTIFIER);

    ast_free(root);
}

static void test_all_optimizations_propagate_then_fold(void) {
    OptimizationOptions options = optimization_options_none();
    ASTNode *root = make_combined_optimization_program();
    ASTNode *result_declaration;

    assert(optimization_enable_by_name(&options, "all"));
    root = optimize_ast(root, &options);
    result_declaration = second_statement(root);

    assert(result_declaration->kind == AST_DECLARATION);
    assert(result_declaration->data.declaration.initializer->kind == AST_NUMBER_LITERAL);
    assert(result_declaration->data.declaration.initializer->data.literal.value[0] == '5');
    assert(result_declaration->data.declaration.initializer->data.literal.value[1] == '\0');

    ast_free(root);
}

static void test_unknown_optimization_is_rejected(void) {
    OptimizationOptions options = optimization_options_none();

    assert(!optimization_enable_by_name(&options, "not-an-optimization"));
    assert(!optimization_disable_by_name(&options, "not-an-optimization"));
}

int main(void) {
    test_no_optimizations_leave_tree_unchanged();
    test_constant_folding_can_be_enabled();
    test_constant_folding_can_be_disabled();
    test_constant_propagation_can_be_enabled();
    test_constant_propagation_can_be_disabled();
    test_all_optimizations_propagate_then_fold();
    test_unknown_optimization_is_rejected();
    return 0;
}
