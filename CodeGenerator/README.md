# Code Generator Round-Trip Test

This folder currently contains a standalone AST loader used to test reconstruction of the AST from `ast.txt`.

## Build

From the `CodeGenerator/` directory:

```bash
gcc code_generator.c ../SemanticRoutines/ast.c -o code_generator
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

The program reconstructs the AST from the text file and prints it in post-order traversal so you can compare it with the original parser output.
