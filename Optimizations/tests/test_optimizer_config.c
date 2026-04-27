#include <assert.h>
#include <string.h>

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

static ASTNode *make_dead_code_program(void) {
    ASTNodeList *statements = NULL;
    ASTNodeList *functions = NULL;

    statements = ast_list_append(
        statements,
        ast_make_declaration(TYPE_INT, "kept", ast_make_number_literal("1", 1), 1)
    );
    statements = ast_list_append(statements, ast_make_return(NULL, 2));
    statements = ast_list_append(
        statements,
        ast_make_declaration(TYPE_INT, "removed", ast_make_number_literal("2", 3), 3)
    );

    functions = ast_list_append(
        functions,
        ast_make_function("setup", TYPE_VOID, ast_make_block(statements, 1), 1)
    );

    return ast_make_program(NULL, functions, 1);
}

static ASTNode *make_algebraic_strength_program(void) {
    ASTNodeList *statements = NULL;
    ASTNodeList *functions = NULL;

    statements = ast_list_append(
        statements,
        ast_make_declaration(TYPE_INT, "x", ast_make_identifier("input", 1), 1)
    );
    statements = ast_list_append(
        statements,
        ast_make_declaration(
            TYPE_INT,
            "zeroed",
            ast_make_binary(OP_MUL, ast_make_identifier("x", 2), ast_make_number_literal("0", 2), 2),
            2
        )
    );
    statements = ast_list_append(
        statements,
        ast_make_declaration(
            TYPE_INT,
            "same",
            ast_make_binary(OP_ADD, ast_make_identifier("x", 3), ast_make_number_literal("0", 3), 3),
            3
        )
    );

    functions = ast_list_append(
        functions,
        ast_make_function("setup", TYPE_VOID, ast_make_block(statements, 1), 1)
    );

    return ast_make_program(NULL, functions, 1);
}

static ASTNode *make_dead_if_program(void) {
    ASTNodeList *then_statements = NULL;
    ASTNodeList *else_statements = NULL;
    ASTNodeList *statements = NULL;
    ASTNodeList *functions = NULL;

    then_statements = ast_list_append(
        then_statements,
        ast_make_declaration(TYPE_INT, "removed", ast_make_number_literal("1", 2), 2)
    );
    else_statements = ast_list_append(
        else_statements,
        ast_make_declaration(TYPE_INT, "kept", ast_make_number_literal("2", 4), 4)
    );
    statements = ast_list_append(
        statements,
        ast_make_if(
            ast_make_bool_literal(0, 1),
            ast_make_block(then_statements, 2),
            ast_make_block(else_statements, 4),
            1
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

static ASTNode *third_statement(ASTNode *program) {
    ASTNode *function = program->data.program.functions->node;
    ASTNode *block = function->data.function.body;
    return block->data.block.statements->next->next->node;
}

static ASTNode *first_statement(ASTNode *program) {
    ASTNode *function = program->data.program.functions->node;
    ASTNode *block = function->data.function.body;
    return block->data.block.statements->node;
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

static void test_dead_code_elimination_removes_unreachable_statement(void) {
    OptimizationOptions options = optimization_options_none();
    ASTNode *root = make_dead_code_program();
    ASTNode *function;
    ASTNode *block;

    assert(optimization_enable_by_name(&options, "dead-code-elimination"));
    root = optimize_ast(root, &options);
    function = root->data.program.functions->node;
    block = function->data.function.body;

    assert(block->data.block.statements != NULL);
    assert(block->data.block.statements->node->kind == AST_DECLARATION);
    assert(block->data.block.statements->next != NULL);
    assert(block->data.block.statements->next->node->kind == AST_RETURN_STMT);
    assert(block->data.block.statements->next->next == NULL);

    ast_free(root);
}

static void test_dead_code_elimination_simplifies_literal_if(void) {
    OptimizationOptions options = optimization_options_none();
    ASTNode *root = make_dead_if_program();
    ASTNode *replacement = NULL;

    assert(optimization_enable_by_name(&options, "dce"));
    root = optimize_ast(root, &options);
    replacement = first_statement(root);

    assert(replacement->kind == AST_BLOCK);
    assert(replacement->data.block.statements != NULL);
    assert(replacement->data.block.statements->node->kind == AST_DECLARATION);
    assert(strcmp(replacement->data.block.statements->node->data.declaration.name, "kept") == 0);

    ast_free(root);
}

static void test_algebraic_strength_reduction_simplifies_identities(void) {
    OptimizationOptions options = optimization_options_none();
    ASTNode *root = make_algebraic_strength_program();
    ASTNode *zeroed_declaration;
    ASTNode *same_declaration;

    assert(optimization_enable_by_name(&options, "algebraic-strength-reduction"));
    root = optimize_ast(root, &options);
    zeroed_declaration = second_statement(root);
    same_declaration = third_statement(root);

    assert(zeroed_declaration->kind == AST_DECLARATION);
    assert(zeroed_declaration->data.declaration.initializer->kind == AST_NUMBER_LITERAL);
    assert(strcmp(zeroed_declaration->data.declaration.initializer->data.literal.value, "0") == 0);

    assert(same_declaration->kind == AST_DECLARATION);
    assert(same_declaration->data.declaration.initializer->kind == AST_IDENTIFIER);
    assert(strcmp(same_declaration->data.declaration.initializer->data.identifier.name, "x") == 0);

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
    test_dead_code_elimination_removes_unreachable_statement();
    test_dead_code_elimination_simplifies_literal_if();
    test_algebraic_strength_reduction_simplifies_identities();
    test_unknown_optimization_is_rejected();
    return 0;
}
