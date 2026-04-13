All of the test prints for the passing tests. All tests can be found in Parser/tests/


Parse successful: tests/pass/test1_basic.c

AST (post-order traversal)
        NumberLiteral(value=5, type=int)
      Declaration(name=x, declared_type=int, type=int)
        BooleanLiteral(value=true, type=bool)
      Declaration(name=ready, declared_type=bool, type=bool)
    Block
  Function(name=setup, return=void)
      Declaration(name=y, declared_type=int, type=int)
        NumberLiteral(value=10, type=int)
      Assignment(name=y, type=int)
    Block
  Function(name=loop, return=void)
Program

Semantic Errors: none

Symbol Tables
Scope #0 (depth=0, label=global)
  setup : function, type=void, declared_at_line=4
  loop : function, type=void, declared_at_line=9
Scope #1 (depth=1, label=setup)
  x : variable, type=int, declared_at_line=2
  ready : variable, type=bool, declared_at_line=3
Scope #2 (depth=1, label=loop)
  y : variable, type=int, declared_at_line=7
Parse successful: tests/pass/test2_serial_delay.c

AST (post-order traversal)
        StringLiteral(value="Starting", type=string)
      ArduinoCall(name=Serial.println, type=void)
        NumberLiteral(value=1000, type=int)
      ArduinoCall(name=delay, type=void)
    Block
  Function(name=setup, return=void)
        StringLiteral(value="Looping", type=string)
      ArduinoCall(name=Serial.print, type=void)
        NumberLiteral(value=500, type=int)
      ArduinoCall(name=delay, type=void)
    Block
  Function(name=loop, return=void)
Program

Semantic Errors: none

Symbol Tables
Scope #0 (depth=0, label=global)
  setup : function, type=void, declared_at_line=4
  loop : function, type=void, declared_at_line=9
Scope #1 (depth=1, label=setup)
  <empty>
Scope #2 (depth=1, label=loop)
  <empty>
Parse successful: tests/pass/test3_if_else.c

AST (post-order traversal)
        NumberLiteral(value=3, type=int)
      Declaration(name=x, declared_type=int, type=int)
          Identifier(name=x, type=int)
          NumberLiteral(value=5, type=int)
        BinaryExpr(op=<, type=bool)
              Identifier(name=x, type=int)
              NumberLiteral(value=1, type=int)
            BinaryExpr(op=+, type=int)
          Assignment(name=x, type=int)
        Block
              Identifier(name=x, type=int)
              NumberLiteral(value=1, type=int)
            BinaryExpr(op=-, type=int)
          Assignment(name=x, type=int)
        Block
      IfStatement
    Block
  Function(name=setup, return=void)
        BooleanLiteral(value=false, type=bool)
      Declaration(name=flag, declared_type=bool, type=bool)
          Identifier(name=flag, type=bool)
        UnaryExpr(op=!, type=bool)
            BooleanLiteral(value=true, type=bool)
          Assignment(name=flag, type=bool)
        Block
            BooleanLiteral(value=false, type=bool)
          Assignment(name=flag, type=bool)
        Block
      IfStatement
    Block
  Function(name=loop, return=void)
Program

Semantic Errors: none

Symbol Tables
Scope #0 (depth=0, label=global)
  setup : function, type=void, declared_at_line=9
  loop : function, type=void, declared_at_line=19
Scope #1 (depth=1, label=setup)
  x : variable, type=int, declared_at_line=2
Scope #2 (depth=2, label=block)
  <empty>
Scope #3 (depth=2, label=block)
  <empty>
Scope #4 (depth=1, label=loop)
  flag : variable, type=bool, declared_at_line=12
Scope #5 (depth=2, label=block)
  <empty>
Scope #6 (depth=2, label=block)
  <empty>
Parse successful: tests/pass/test4_semantic_ok.c

AST (post-order traversal)
  Include(name=Arduino)
          NumberLiteral(value=0, type=int)
        ArduinoCall(name=analogRead, type=int)
      Declaration(name=sensor, declared_type=int, type=int)
        BooleanLiteral(value=true, type=bool)
      Declaration(name=ready, declared_type=bool, type=bool)
            Identifier(name=sensor, type=int)
            NumberLiteral(value=10, type=int)
          BinaryExpr(op=>, type=bool)
          Identifier(name=ready, type=bool)
        BinaryExpr(op=&&, type=bool)
            StringLiteral(value="sensor ready", type=string)
          ArduinoCall(name=Serial.println, type=void)
        Block
      IfStatement
    Block
  Function(name=setup, return=void)
      Declaration(name=value, declared_type=int, type=int)
            NumberLiteral(value=2, type=int)
          ArduinoCall(name=digitalRead, type=int)
          NumberLiteral(value=1, type=int)
        BinaryExpr(op=+, type=int)
      Assignment(name=value, type=int)
          Identifier(name=value, type=int)
          NumberLiteral(value=4, type=int)
        BinaryExpr(op=<, type=bool)
              Identifier(name=value, type=int)
              NumberLiteral(value=1, type=int)
            BinaryExpr(op=+, type=int)
          Assignment(name=value, type=int)
        Block
      WhileStatement
    Block
  Function(name=loop, return=void)
Program

Semantic Errors: none

Symbol Tables
Scope #0 (depth=0, label=global)
  setup : function, type=void, declared_at_line=10
  loop : function, type=void, declared_at_line=19
Scope #1 (depth=1, label=setup)
  sensor : variable, type=int, declared_at_line=4
  ready : variable, type=bool, declared_at_line=5
Scope #2 (depth=2, label=block)
  <empty>
Scope #3 (depth=1, label=loop)
  value : variable, type=int, declared_at_line=13
Scope #4 (depth=2, label=block)
  <empty>
Parse failed: tests/fail/test1_incomplete_expr.c

Same thing for all of the other failing tests.