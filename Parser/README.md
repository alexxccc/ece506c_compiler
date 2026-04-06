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