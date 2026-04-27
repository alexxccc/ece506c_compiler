#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Optimizations/optimizer.h"
#include "../SemanticRoutines/ast.h"

static int g_label_counter = 0;
static int g_string_counter = 0;
static int g_expression_counter = 0;

static void emit_line(FILE *out, const char *format, ...) {
    va_list args;

    va_start(args, format);
    vfprintf(out, format, args);
    va_end(args);
}

static int next_label_id(void) {
    return g_label_counter++;
}

static void generate_expression(ASTNode *node, FILE *out);
static void generate_expression_tests(ASTNode *node, FILE *out);

static void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [options] [ast-file]\n", program_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  --opt=NAME          Enable an optimization by name\n");
    fprintf(stderr, "  --disable-opt=NAME  Disable an optimization by name\n");
    fprintf(stderr, "  --no-opt            Disable all optimizations\n");
    fprintf(stderr, "  --list-opts         Print available optimization names\n");
    fprintf(stderr, "\nAvailable optimizations:\n");
    optimization_print_available(stderr);
}

static int parse_arguments(
    int argc,
    char **argv,
    const char **input_path_out,
    OptimizationOptions *options_out
) {
    int i;
    int input_was_provided = 0;

    if (input_path_out == NULL || options_out == NULL) {
        return 0;
    }

    *input_path_out = "ast.txt";
    *options_out = optimization_options_none();

    for (i = 1; i < argc; ++i) {
        const char *argument = argv[i];

        if (strcmp(argument, "--help") == 0 || strcmp(argument, "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        }

        if (strcmp(argument, "--list-opts") == 0) {
            optimization_print_available(stdout);
            return 0;
        }

        if (strcmp(argument, "--no-opt") == 0 || strcmp(argument, "--opt=none") == 0) {
            *options_out = optimization_options_none();
            continue;
        }

        if (strncmp(argument, "--opt=", 6) == 0) {
            if (!optimization_enable_by_name(options_out, argument + 6)) {
                fprintf(stderr, "Unknown optimization: %s\n", argument + 6);
                return -1;
            }
            continue;
        }

        if (strncmp(argument, "--disable-opt=", 14) == 0) {
            if (!optimization_disable_by_name(options_out, argument + 14)) {
                fprintf(stderr, "Unknown optimization: %s\n", argument + 14);
                return -1;
            }
            continue;
        }

        if (argument[0] == '-') {
            fprintf(stderr, "Unknown option: %s\n", argument);
            return -1;
        }

        if (input_was_provided) {
            fprintf(stderr, "Only one AST input file can be provided.\n");
            return -1;
        }

        *input_path_out = argument;
        input_was_provided = 1;
    }

    return 1;
}

static void emit_push_result(FILE *out) {
    emit_line(out, "    push r25\n");
    emit_line(out, "    push r24\n");
}

static void emit_pop_left_operand(FILE *out) {
    emit_line(out, "    pop r18\n");
    emit_line(out, "    pop r19\n");
}

static void generate_number_literal(ASTNode *node, FILE *out) {
    long value = strtol(node->data.literal.value, NULL, 10);

    emit_line(out, "    ; load integer literal %s\n", node->data.literal.value);
    emit_line(out, "    ldi r24, lo8(%ld)\n", value);
    emit_line(out, "    ldi r25, hi8(%ld)\n", value);
}

static void generate_bool_literal(ASTNode *node, FILE *out) {
    emit_line(out, "    ; load boolean literal %s\n", node->data.literal.bool_value ? "true" : "false");
    emit_line(out, "    ldi r24, %d\n", node->data.literal.bool_value ? 1 : 0);
    emit_line(out, "    clr r25\n");
}

static void generate_string_literal(ASTNode *node, FILE *out) {
    int string_id = g_string_counter++;

    emit_line(out, "    ; load address of string literal %s\n", node->data.literal.value);
    emit_line(out, "    ldi r24, lo8(.LC%d)\n", string_id);
    emit_line(out, "    ldi r25, hi8(.LC%d)\n", string_id);
    emit_line(out, "    ; .LC%d: %s\n", string_id, node->data.literal.value);
}

static void generate_identifier(ASTNode *node, FILE *out) {
    emit_line(out, "    ; load variable %s\n", node->data.identifier.name);
    emit_line(out, "    lds r24, %s\n", node->data.identifier.name);
    emit_line(out, "    lds r25, %s+1\n", node->data.identifier.name);
}

static void generate_unary_expression(ASTNode *node, FILE *out) {
    int true_label = next_label_id();
    int end_label = next_label_id();

    generate_expression(node->data.unary_expr.operand, out);

    switch (node->data.unary_expr.op) {
        case OP_NOT:
        default:
            emit_line(out, "    ; logical not\n");
            emit_line(out, "    or r24, r25\n");
            emit_line(out, "    breq .Lexpr_true_%d\n", true_label);
            emit_line(out, "    clr r24\n");
            emit_line(out, "    clr r25\n");
            emit_line(out, "    rjmp .Lexpr_end_%d\n", end_label);
            emit_line(out, ".Lexpr_true_%d:\n", true_label);
            emit_line(out, "    ldi r24, 1\n");
            emit_line(out, "    clr r25\n");
            emit_line(out, ".Lexpr_end_%d:\n", end_label);
            break;
    }
}

static void generate_arduino_expression(ASTNode *node, FILE *out) {
    if (node->data.arduino_call.first_arg != NULL) {
        generate_expression(node->data.arduino_call.first_arg, out);
    }
    if (node->data.arduino_call.second_arg != NULL) {
        emit_push_result(out);
        generate_expression(node->data.arduino_call.second_arg, out);
        emit_pop_left_operand(out);
        emit_line(out, "    ; second Arduino argument is in r24:r25, first was preserved in r18:r19\n");
    }

    switch (node->data.arduino_call.call_kind) {
        case CALL_DIGITAL_READ:
            emit_line(out, "    ; call digitalRead(pin)\n");
            emit_line(out, "    call digitalRead\n");
            break;
        case CALL_ANALOG_READ:
            emit_line(out, "    ; call analogRead(pin)\n");
            emit_line(out, "    call analogRead\n");
            break;
        case CALL_SERIAL_PRINT:
            emit_line(out, "    ; call Serial.print(value)\n");
            emit_line(out, "    call Serial_print\n");
            break;
        case CALL_SERIAL_PRINTLN:
            emit_line(out, "    ; call Serial.println(value)\n");
            emit_line(out, "    call Serial_println\n");
            break;
        case CALL_DELAY:
            emit_line(out, "    ; call delay(milliseconds)\n");
            emit_line(out, "    call delay\n");
            break;
        case CALL_PINMODE:
            emit_line(out, "    ; call pinMode(pin, mode)\n");
            emit_line(out, "    call pinMode\n");
            break;
        case CALL_DIGITAL_WRITE:
            emit_line(out, "    ; call digitalWrite(pin, value)\n");
            emit_line(out, "    call digitalWrite\n");
            break;
        case CALL_ANALOG_WRITE:
            emit_line(out, "    ; call analogWrite(pin, value)\n");
            emit_line(out, "    call analogWrite\n");
            break;
        default:
            emit_line(out, "    ; unsupported Arduino call\n");
            break;
    }
}

static void generate_binary_expression(ASTNode *node, FILE *out) {
    int label_a = next_label_id();
    int label_b = next_label_id();

    generate_expression(node->data.binary_expr.left, out);
    emit_push_result(out);
    generate_expression(node->data.binary_expr.right, out);
    emit_pop_left_operand(out);

    switch (node->data.binary_expr.op) {
        case OP_ADD:
            emit_line(out, "    ; add left operand (r19:r18) to right operand (r25:r24)\n");
            emit_line(out, "    add r24, r18\n");
            emit_line(out, "    adc r25, r19\n");
            break;
        case OP_SUB:
            emit_line(out, "    ; subtract right operand from left operand\n");
            emit_line(out, "    sub r18, r24\n");
            emit_line(out, "    sbc r19, r25\n");
            emit_line(out, "    mov r24, r18\n");
            emit_line(out, "    mov r25, r19\n");
            break;
        case OP_MUL:
            emit_line(out, "    ; multiply 16-bit operands using runtime helper\n");
            emit_line(out, "    mov r22, r18\n");
            emit_line(out, "    mov r23, r19\n");
            emit_line(out, "    call __mulhi3\n");
            break;
        case OP_DIV:
            emit_line(out, "    ; divide 16-bit operands using runtime helper\n");
            emit_line(out, "    mov r22, r18\n");
            emit_line(out, "    mov r23, r19\n");
            emit_line(out, "    call __divhi3\n");
            break;
        case OP_MOD:
            emit_line(out, "    ; modulo 16-bit operands using runtime helper\n");
            emit_line(out, "    mov r22, r18\n");
            emit_line(out, "    mov r23, r19\n");
            emit_line(out, "    call __modhi3\n");
            break;
        case OP_EQ:
            emit_line(out, "    ; compare equality\n");
            emit_line(out, "    cp r18, r24\n");
            emit_line(out, "    cpc r19, r25\n");
            emit_line(out, "    breq .Lexpr_true_%d\n", label_a);
            emit_line(out, "    clr r24\n");
            emit_line(out, "    clr r25\n");
            emit_line(out, "    rjmp .Lexpr_end_%d\n", label_b);
            emit_line(out, ".Lexpr_true_%d:\n", label_a);
            emit_line(out, "    ldi r24, 1\n");
            emit_line(out, "    clr r25\n");
            emit_line(out, ".Lexpr_end_%d:\n", label_b);
            break;
        case OP_NEQ:
            emit_line(out, "    ; compare inequality\n");
            emit_line(out, "    cp r18, r24\n");
            emit_line(out, "    cpc r19, r25\n");
            emit_line(out, "    brne .Lexpr_true_%d\n", label_a);
            emit_line(out, "    clr r24\n");
            emit_line(out, "    clr r25\n");
            emit_line(out, "    rjmp .Lexpr_end_%d\n", label_b);
            emit_line(out, ".Lexpr_true_%d:\n", label_a);
            emit_line(out, "    ldi r24, 1\n");
            emit_line(out, "    clr r25\n");
            emit_line(out, ".Lexpr_end_%d:\n", label_b);
            break;
        case OP_LT:
            emit_line(out, "    ; compare left < right\n");
            emit_line(out, "    cp r18, r24\n");
            emit_line(out, "    cpc r19, r25\n");
            emit_line(out, "    brlt .Lexpr_true_%d\n", label_a);
            emit_line(out, "    clr r24\n");
            emit_line(out, "    clr r25\n");
            emit_line(out, "    rjmp .Lexpr_end_%d\n", label_b);
            emit_line(out, ".Lexpr_true_%d:\n", label_a);
            emit_line(out, "    ldi r24, 1\n");
            emit_line(out, "    clr r25\n");
            emit_line(out, ".Lexpr_end_%d:\n", label_b);
            break;
        case OP_LTE:
            emit_line(out, "    ; compare left <= right\n");
            emit_line(out, "    cp r18, r24\n");
            emit_line(out, "    cpc r19, r25\n");
            emit_line(out, "    brlt .Lexpr_true_%d\n", label_a);
            emit_line(out, "    breq .Lexpr_true_%d\n", label_a);
            emit_line(out, "    clr r24\n");
            emit_line(out, "    clr r25\n");
            emit_line(out, "    rjmp .Lexpr_end_%d\n", label_b);
            emit_line(out, ".Lexpr_true_%d:\n", label_a);
            emit_line(out, "    ldi r24, 1\n");
            emit_line(out, "    clr r25\n");
            emit_line(out, ".Lexpr_end_%d:\n", label_b);
            break;
        case OP_GT:
            emit_line(out, "    ; compare left > right\n");
            emit_line(out, "    cp r18, r24\n");
            emit_line(out, "    cpc r19, r25\n");
            emit_line(out, "    brlt .Lexpr_false_%d\n", label_a);
            emit_line(out, "    breq .Lexpr_false_%d\n", label_a);
            emit_line(out, "    ldi r24, 1\n");
            emit_line(out, "    clr r25\n");
            emit_line(out, "    rjmp .Lexpr_end_%d\n", label_b);
            emit_line(out, ".Lexpr_false_%d:\n", label_a);
            emit_line(out, "    clr r24\n");
            emit_line(out, "    clr r25\n");
            emit_line(out, ".Lexpr_end_%d:\n", label_b);
            break;
        case OP_GTE:
            emit_line(out, "    ; compare left >= right\n");
            emit_line(out, "    cp r18, r24\n");
            emit_line(out, "    cpc r19, r25\n");
            emit_line(out, "    brlt .Lexpr_false_%d\n", label_a);
            emit_line(out, "    ldi r24, 1\n");
            emit_line(out, "    clr r25\n");
            emit_line(out, "    rjmp .Lexpr_end_%d\n", label_b);
            emit_line(out, ".Lexpr_false_%d:\n", label_a);
            emit_line(out, "    clr r24\n");
            emit_line(out, "    clr r25\n");
            emit_line(out, ".Lexpr_end_%d:\n", label_b);
            break;
        case OP_AND:
            emit_line(out, "    ; logical and\n");
            emit_line(out, "    or r18, r19\n");
            emit_line(out, "    breq .Lexpr_false_%d\n", label_a);
            emit_line(out, "    or r24, r25\n");
            emit_line(out, "    breq .Lexpr_false_%d\n", label_a);
            emit_line(out, "    ldi r24, 1\n");
            emit_line(out, "    clr r25\n");
            emit_line(out, "    rjmp .Lexpr_end_%d\n", label_b);
            emit_line(out, ".Lexpr_false_%d:\n", label_a);
            emit_line(out, "    clr r24\n");
            emit_line(out, "    clr r25\n");
            emit_line(out, ".Lexpr_end_%d:\n", label_b);
            break;
        case OP_OR:
            emit_line(out, "    ; logical or\n");
            emit_line(out, "    or r18, r19\n");
            emit_line(out, "    brne .Lexpr_true_%d\n", label_a);
            emit_line(out, "    or r24, r25\n");
            emit_line(out, "    brne .Lexpr_true_%d\n", label_a);
            emit_line(out, "    clr r24\n");
            emit_line(out, "    clr r25\n");
            emit_line(out, "    rjmp .Lexpr_end_%d\n", label_b);
            emit_line(out, ".Lexpr_true_%d:\n", label_a);
            emit_line(out, "    ldi r24, 1\n");
            emit_line(out, "    clr r25\n");
            emit_line(out, ".Lexpr_end_%d:\n", label_b);
            break;
        default:
            emit_line(out, "    ; unsupported binary expression\n");
            break;
    }
}

static void generate_expression(ASTNode *node, FILE *out) {
    if (node == NULL) {
        emit_line(out, "    ; null expression\n");
        emit_line(out, "    clr r24\n");
        emit_line(out, "    clr r25\n");
        return;
    }

    switch (node->kind) {
        case AST_NUMBER_LITERAL:
            generate_number_literal(node, out);
            break;
        case AST_BOOL_LITERAL:
            generate_bool_literal(node, out);
            break;
        case AST_STRING_LITERAL:
            generate_string_literal(node, out);
            break;
        case AST_IDENTIFIER:
            generate_identifier(node, out);
            break;
        case AST_UNARY_EXPR:
            generate_unary_expression(node, out);
            break;
        case AST_BINARY_EXPR:
            generate_binary_expression(node, out);
            break;
        case AST_ARDUINO_CALL:
            generate_arduino_expression(node, out);
            break;
        default:
            emit_line(out, "    ; unsupported expression kind %d\n", node->kind);
            emit_line(out, "    clr r24\n");
            emit_line(out, "    clr r25\n");
            break;
    }
}

static void emit_expression_header(const char *context, FILE *out) {
    ++g_expression_counter;
    emit_line(out, "; Expression %d: %s\n", g_expression_counter, context);
    emit_line(out, "; Result convention: r24:r25 holds the final value\n");
}

static void generate_expression_tests_for_block(ASTNode *block_node, FILE *out) {
    ASTNodeList *cursor;

    if (block_node == NULL || block_node->kind != AST_BLOCK) {
        return;
    }

    cursor = block_node->data.block.statements;
    while (cursor != NULL) {
        generate_expression_tests(cursor->node, out);
        cursor = cursor->next;
    }
}

static void generate_expression_tests(ASTNode *node, FILE *out) {
    if (node == NULL) {
        return;
    }

    switch (node->kind) {
        case AST_PROGRAM: {
            ASTNodeList *functions = node->data.program.functions;
            while (functions != NULL) {
                generate_expression_tests(functions->node, out);
                functions = functions->next;
            }
            break;
        }
        case AST_FUNCTION:
            emit_line(out, "\n; ===== Function %s =====\n", node->data.function.name);
            generate_expression_tests_for_block(node->data.function.body, out);
            break;
        case AST_BLOCK:
            generate_expression_tests_for_block(node, out);
            break;
        case AST_DECLARATION:
            if (node->data.declaration.initializer != NULL) {
                emit_expression_header("declaration initializer", out);
                generate_expression(node->data.declaration.initializer, out);
                emit_line(out, "\n");
            }
            break;
        case AST_ASSIGNMENT:
            emit_expression_header("assignment value", out);
            generate_expression(node->data.assignment.value, out);
            emit_line(out, "\n");
            break;
        case AST_IF_STMT:
            emit_expression_header("if condition", out);
            generate_expression(node->data.if_stmt.condition, out);
            emit_line(out, "\n");
            generate_expression_tests(node->data.if_stmt.then_branch, out);
            generate_expression_tests(node->data.if_stmt.else_branch, out);
            break;
        case AST_WHILE_STMT:
            emit_expression_header("while condition", out);
            generate_expression(node->data.while_stmt.condition, out);
            emit_line(out, "\n");
            generate_expression_tests(node->data.while_stmt.body, out);
            break;
        case AST_RETURN_STMT:
            if (node->data.return_stmt.value != NULL) {
                emit_expression_header("return value", out);
                generate_expression(node->data.return_stmt.value, out);
                emit_line(out, "\n");
            }
            break;
        case AST_ARDUINO_CALL:
            emit_expression_header("Arduino call statement", out);
            generate_expression(node, out);
            emit_line(out, "\n");
            break;
        default:
            break;
    }
}

int main(int argc, char **argv) {
    const char *input_path;
    OptimizationOptions optimization_options;
    ASTNode *root;
    const char *output_path = "assembly_output.asm";
    FILE *assembly_out;
    int parse_result;

    parse_result = parse_arguments(argc, argv, &input_path, &optimization_options);
    if (parse_result <= 0) {
        return (parse_result < 0) ? 1 : 0;
    }

    root = ast_read_post_order_file(input_path);

    if (root == NULL && strcmp(input_path, "ast.txt") == 0) {
        input_path = "../Parser/ast.txt";
        root = ast_read_post_order_file(input_path);
    }

    if (root == NULL) {
        fprintf(stderr, "Could not reconstruct AST from %s\n", input_path);
        return 1;
    }

    root = optimize_ast(root, &optimization_options);

    printf("Reconstructed AST from %s\n", input_path);
    printf("Enabled optimizations: ");
    optimization_print_enabled(&optimization_options, stdout);
    printf("\n");
    printf("AST (post-order traversal)\n");
    ast_print_post_order(root, stdout);
    printf("\n\n");

    assembly_out = fopen(output_path, "w");
    if (assembly_out == NULL) {
        fprintf(stderr, "Could not open %s for writing\n", output_path);
        ast_free(root);
        return 1;
    }

    fprintf(assembly_out, "; Generated from %s\n", input_path);
    fprintf(assembly_out, "; Expression Code Generation (AVR-style)\n");
    fprintf(assembly_out, "; Enabled optimizations: ");
    optimization_print_enabled(&optimization_options, assembly_out);
    fprintf(assembly_out, "\n");
    generate_expression_tests(root, assembly_out);
    fclose(assembly_out);

    printf("Assembly written to %s\n", output_path);

    ast_free(root);
    return 0;
}
