# Parser (Bison + Flex Implementation)

## Overview
This parser implements the syntax analysis phase for a restricted subset of an Arduino-style C++ language.

It uses:
- **Flex** for lexical analysis (scanner)
- **Bison** for syntax analysis (parser)

The parser validates whether an input program conforms to the defined grammar.

---

## Files

- `parser.y` – Bison grammar specification
- `scanner.l` – Flex lexical analyzer
- `main.c` – Driver program that runs the parser
- `token.h` – Token definitions
- `tests/`
  - `pass/` – Valid programs (should parse successfully)
  - `fail/` – Invalid programs (should fail parsing)

---

## Build Instructions

From the `Parser/` directory:

```bash
bison -d parser.y
flex scanner.l
gcc parser.tab.c lex.yy.c main.c -o parser
```

## Run Instructions

In ./Parser

parser tests/pass/test1_basic.c
parser tests/pass/test2_serial_delay.c
parser tests/pass/test3_if_else.c

parser tests/fail/test1_incomplete_expr.c
parser tests/fail/test2_bad_token.c
