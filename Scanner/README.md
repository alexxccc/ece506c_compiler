# Scanner (Flex Implementation)

## Overview
This scanner performs lexical analysis for a restricted subset of an Arduino-style C++ language.  
It tokenizes the input source file into keywords, identifiers, numbers, operators, punctuation, and string literals.

Whitespace and single-line comments (`//`) are ignored.

---

## Files
- `scanner.l` – Flex specification for the scanner
- `main.c` – Driver program that repeatedly calls `yylex()`
- `token.h` – Token enumeration definitions
- `tests/` – Directory containing test files

---

## Build Instructions

From the `Scanner/` directory:

```bash
flex -o scanner.c scanner.l
gcc scanner.c main.c -o scanner
```

## Running the Scanner:

### To Run with Specific Test:
```bash
./scanner $location_of_test
```
### Example
```bash
./scanner tests/program.c
```
### To Run Default Test:
```bash
./scanner
```
Note:
If no test location is specified it will default to using tests/program.c
