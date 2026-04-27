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

static void test_unknown_optimization_is_rejected(void) {
    OptimizationOptions options = optimization_options_none();

    assert(!optimization_enable_by_name(&options, "not-an-optimization"));
    assert(!optimization_disable_by_name(&options, "not-an-optimization"));
}

int main(void) {
    test_no_optimizations_leave_tree_unchanged();
    test_constant_folding_can_be_enabled();
    test_constant_folding_can_be_disabled();
    test_unknown_optimization_is_rejected();
    return 0;
}
