# Context-Free Grammar Specification


Write the description of your context-free grammar here.

This context-free grammar defines the syntax of a small subset of the Arduino Sketch language. 

The grammar supports variable declarations, assignments, arithmetic and boolean expressions, conditional statements (`if` / `else`), loop constructs (`while`), and basic Arduino function calls such as `Serial.print()` and `Serial.println()`. 

The programs are composed of sequences of statements that may include blocks sorrounded by `{}`. Expressions support arithmetic operators (`+`, `-`, `*`, `/`, `%`), comparison operators (`==`, `!=`, `<`, `>`, `<=`, `>=`), and boolean operators (`&&`, `||`, `!`). Numeric literals, string literals, and identifiers are allowed as expression values.

## Formal Definition

The grammar G is a 4-tuple (V, Σ, R, S):

### V (Variables/Non-terminals):

Write the Non-terminals set here.

V = {
  Program,
  StmtList,
  Stmt,
  Decl,
  Assign,
  IfStmt,
  WhileStmt,
  Block,
  Expr,
  Term,
  Factor
}

### Σ (Terminals):

Write the Terminals set here.

Σ = {
  int, bool, if, else, while, true, false,
  Serial, print, println,

  =, +, -, *, /, %,
  ==, !=, <, >, <=, >=,
  &&, ||, !,

  ;, ,, ., (, ), {, },

  identifier, number, string
}

### S (Start Symbol):

Write the Start Symbol here.

S = Program

### R (Production Rules):

Program --> StmtList

StmtList --> StmtList Stmt
StmtList --> Stmt

Stmt --> Decl ;
Stmt --> Assign ;
Stmt --> IfStmt
Stmt --> WhileStmt
Stmt --> Block
Stmt --> Expr ;

Decl --> int identifier
Decl --> bool identifier

Assign --> identifier = Expr

IfStmt --> if ( Expr ) Stmt
IfStmt --> if ( Expr ) Stmt else Stmt

WhileStmt --> while ( Expr ) Stmt

Block --> { StmtList }

Expr --> Expr + Term
Expr --> Expr - Term
Expr --> Term

Term --> Term * Factor
Term --> Term / Factor
Term --> Term % Factor
Term --> Factor

Factor --> ( Expr )
Factor --> identifier
Factor --> number
Factor --> string
Factor --> true
Factor --> false
Factor --> ! Factor


## LR(1) Automaton


Draw the LR(1) automaton for your grammar
![Alt text][def]


## LALR Verification

### Item Sets: 

List the states that can be merged 
States 8 and 12 can be merged. States 7 and 11 can be merged.

### Parse Table and Conflict Check

Draw the LALR Parse Table and check if there are any shift-reduce conflict. Write down if you found or not any shift-reduce conflict.

| State  | **ACTION** |          |     |     |      |      | **GOTO**  |        |        |        |
| ------ | ---------- | -------- | --- | --- | ---- | ---- | --------- | ------ | ------ | ------ |
|        | id         | number   | =   | +   | ;    | $    | Program   | Stmt   | Expr   | Term   |
| ------ | ----       | -------- | --- | --- | ---- | ---- | --------- | ------ | ------ | ------ |
| I0     | s3         |          |     |     |      |      | I1        | I2     |        |        |
| I1     |            |          |     |     |      | acc  |           |        |        |        |
| I2     |            |          |     |     |      | r1   |           |        |        |        |
| I3     |            |          | s4  |     |      |      |           |        |        |        |
| I4     | s7         | s8       |     |     |      |      |           |        | I5     | I6     |
| I5     |            |          |     | s10 | s9   |      |           |        |        |        |
| I6     |            |          |     | r4  | r4   |      |           |        |        |        |
| I7     |            |          |     | r5  | r5   |      |           |        |        |        |
| I8     |            |          |     | r6  | r6   |      |           |        |        |        |
| I9     |            |          |     |     |      | r2   |           |        |        |        |
| I10    | s7         | s8       |     |     |      |      |           |        |        | I13    |
| I11    |            |          |     | r5  | r5   |      |           |        |        |        |
| I12    |            |          |     | r6  | r6   |      |           |        |        |        |
| I13    |            |          |     | r3  | r3   |      |           |        |        |        |

There are not any shift-reduce conflicts in this table.

### LALR Automaton

Re-draw the LR(1) automaton after merging states





[def]: ./automaton/LR1automaton.png