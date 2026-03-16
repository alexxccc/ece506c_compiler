# Context-Free Grammar Specification


Write the description of your context-free grammar here.

Example: *This grammar defines simple arithmetic expressions with addition, subtraction, multiplication, division, and numbers.*


## Formal Definition

The grammar G is a 4-tuple (V, Σ, R, S):

### V (Variables/Non-terminals):

Write the Non-terminals set here.

Example: `V = {E, T, F, N}`

### Σ (Terminals):

Write the Terminals set here.

Example: `Σ = {+, -, *, /, (, ), number}`

### S (Start Symbol):

Write the Start Symbol here.

Example: `S = {E}`

### R (Production Rules):

```ebnf
E --> T 
E --> E + T 
E --> E - T
T --> F 
T --> T * F 
T --> T / F
F --> N 
F --> (E)
N --> number
```


## LR(1) Automaton


Draw the LR(1) automaton for your grammar


## LALR Verification

### Item Sets: 

List the states that can be merged


### Parse Table and Conflict Check

Draw the LALR Parse Table and check if there are any shift-reduce conflict. Write down if you found or not any shift-reduce conflict.


### LALR Automaton

Re-draw the LR(1) automaton after merging states



