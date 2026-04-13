# Parser (Bison + Flex Implementation)

## Overview
This parser now implements both syntax analysis and a first semantic-analysis pass for a restricted subset of an Arduino-style C++ language.

It uses:
- **Flex** for lexical analysis (scanner)
- **Bison** for syntax analysis (parser)
- **SemanticRoutines/** for AST construction, symbol tables, name resolution, and type checking

The parser validates whether an input program conforms to the defined grammar, builds an AST during parsing, prints the AST in post-order, and then prints semantic-analysis results and symbol tables.

---

## Files

- `parser.y` - Bison grammar specification and AST-construction actions
- `scanner.l` - Flex lexical analyzer
- `main.c` - Driver program that runs the parser and prints the AST / symbol tables
- `token.h` - Token definitions
- `../SemanticRoutines/ast.h` / `ast.c` - AST structures, constructors, printer, and cleanup
- `../SemanticRoutines/semantic.h` / `semantic.c` - Symbol tables, name resolution, type checking, and semantic errors
- `tests/`
  - `pass/` - Valid programs that should parse and pass semantic analysis
  - `fail/` - Programs that should fail syntax or semantic analysis

---

## Build Instructions

From the `Parser/` directory:

```bash
bison -d parser.y
flex scanner.l
gcc parser.tab.c lex.yy.c main.c ../SemanticRoutines/ast.c ../SemanticRoutines/semantic.c -o parser
```

## Run Instructions

In `./Parser`:

```bash
parser tests/pass/test1_basic.c
parser tests/pass/test2_serial_delay.c
parser tests/pass/test3_if_else.c
parser tests/pass/test4_semantic_ok.c

parser tests/fail/test1_incomplete_expr.c
parser tests/fail/test2_bad_token.c
parser tests/fail/test3_undeclared_semantic.c
parser tests/fail/test4_type_mismatch_semantic.c
parser tests/fail/test5_redeclaration_semantic.c
```

## Output

For a successful parse, the driver prints:

1. `Parse successful: ...`
2. The AST in post-order traversal
3. Semantic errors, if any
4. The symbol tables for each scope

Semantic failures still produce an AST and symbol tables so the analysis phase can be inspected.
