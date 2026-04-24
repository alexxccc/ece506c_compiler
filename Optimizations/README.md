# Optimizations

This directory contains standalone optimization passes that run between AST reconstruction and code generation.

## Current passes

- `constant_folding.c` / `constant_folding.h`
  - folds compile-time integer arithmetic
  - folds compile-time comparisons
  - folds compile-time boolean expressions

## Build impact

Any executable that wants optimization support should compile and link the needed files from this directory.
