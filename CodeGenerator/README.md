# Code Generator Round-Trip Test

This folder currently contains a standalone AST loader plus an expression code generator that reads `ast.txt`.
Before emitting assembly, it runs the configured optimization passes from `../Optimizations/`.

## Build

From the `CodeGenerator/` directory:

```bash
gcc code_generator.c ../Optimizations/optimizer.c ../Optimizations/constant_propagation.c ../Optimizations/constant_folding.c ../SemanticRoutines/ast.c -o code_generator
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

Optimizations are disabled by default. Use flags to enable or control them:

```bash
./code_generator --opt=constant-folding ../Parser/ast.txt
./code_generator --opt=constant-propagation ../Parser/ast.txt
./code_generator --opt=all ../Parser/ast.txt
./code_generator --no-opt ../Parser/ast.txt
./code_generator --disable-opt=constant-folding ../Parser/ast.txt
./code_generator --disable-opt=constant-propagation ../Parser/ast.txt
./code_generator --list-opts
```

The program reconstructs the AST from the text file, prints it in post-order traversal, and then walks the tree to emit AVR-style assembly snippets into `assembly_output.asm` for expression nodes such as:

- declaration initializers
- assignment right-hand sides
- `if` / `while` conditions
- return values
- expression-form Arduino calls like `digitalRead(...)`

Available optimization passes are:

- `constant-propagation`
  - replaces uses of variables that currently hold known integer or boolean literals
  - handles straight-line declarations and assignments
  - treats branches and loops conservatively
- `constant-folding`
  - simplifies integer arithmetic on literal operands
  - simplifies integer comparisons on literal operands
  - simplifies boolean `!`, `&&`, and `||` when operands are compile-time literals
  - simplifies equality and inequality checks when both operands are compile-time literals

When both are enabled with `--opt=all`, constant propagation runs before constant folding.
