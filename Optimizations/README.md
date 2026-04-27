# Optimizations

This directory contains standalone optimization passes that run between AST reconstruction and code generation.

## Current passes

- `constant_folding.c` / `constant_folding.h`
  - folds compile-time integer arithmetic
  - folds compile-time comparisons
  - folds compile-time boolean expressions
- `constant_propagation.c` / `constant_propagation.h`
  - propagates known integer and boolean literals through straight-line code
  - replaces identifier uses with known literal values
  - invalidates values conservatively across branches and loops
- `optimizer.c` / `optimizer.h`
  - stores enabled optimization flags
  - enables or disables passes by name
  - runs enabled passes in one central pipeline

## Build impact

Any executable that wants optimization support should compile and link the needed files from this directory.

For code generation, link both the optimizer pipeline and the optimization pass implementations:

```bash
cd ../CodeGenerator
gcc code_generator.c ../Optimizations/optimizer.c ../Optimizations/constant_propagation.c ../Optimizations/constant_folding.c ../SemanticRoutines/ast.c -o code_generator
```

## Tests

The optimizer configuration tests live in `tests/test_optimizer_config.c`.

```bash
gcc tests/test_optimizer_config.c optimizer.c constant_propagation.c constant_folding.c ../SemanticRoutines/ast.c -o test_optimizer_config
./test_optimizer_config
```
