List of potential optimizations for project (ordered)

1. Constant folding - allows for expressions to be evaluated, which minimizes the number of tokens added to tree. Ex: int i = 5 + 7; -> int i = 12;

2. Constant propagation - allows for variables with constant values to be treated as constants unless changed. Ex: 
```
int i = 5;
int y = i + 5; 
```
can be interpreted as:
```
int i = 5;
int y = 5 + 5;
```
In conjunection w/ constant folding, this evaluates to:
```
int i = 5;
int y = 10;
```

3. Algebraic + Logical Simplification
4. Dead code elimination

3. and 4. likely require 1. and 2. to be implemented easily.

5. Strength reduction