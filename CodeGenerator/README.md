# Code Generator Round-Trip Test

This folder currently contains a standalone AST loader plus an expression code generator that reads `ast.txt`.
Before emitting assembly, it runs a constant-folding optimization pass from `../Optimizations/`.

## Build

From the `CodeGenerator/` directory:

```bash
gcc code_generator.c ../Optimizations/constant_folding.c ../SemanticRoutines/ast.c -o code_generator
```

## Run

If `ast.txt` is in the current directory:

```bash
./code_generator
```

Or pass the AST file explicitly:

```bash
./code_generator ../Parser/ast.txt
```

The program reconstructs the AST from the text file, prints it in post-order traversal, and then walks the tree to emit AVR-style assembly snippets into `assembly_output.asm` for expression nodes such as:

- declaration initializers
- assignment right-hand sides
- `if` / `while` conditions
- return values
- expression-form Arduino calls like `digitalRead(...)`

The constant-folding pass currently simplifies:

- integer arithmetic on literal operands
- integer comparisons on literal operands
- boolean `!`, `&&`, and `||` when operands are compile-time literals
- equality and inequality checks when both operands are compile-time literals
