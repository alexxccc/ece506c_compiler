## test1_basic.c

; Generated from ../Parser/ast.txt
; Expression Code Generation (AVR-style)

; ===== Function setup =====
; Expression 1: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load integer literal 5
    ldi r24, lo8(5)
    ldi r25, hi8(5)

; Expression 2: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load boolean literal true
    ldi r24, 1
    clr r25


; ===== Function loop =====
; Expression 3: assignment value
; Result convention: r24:r25 holds the final value
    ; load integer literal 10
    ldi r24, lo8(10)
    ldi r25, hi8(10)

## test2_serial_delay.c

; Generated from ../Parser/ast.txt
; Expression Code Generation (AVR-style)

; ===== Function setup =====
; Expression 1: Arduino call statement
; Result convention: r24:r25 holds the final value
    ; load address of string literal "Starting"
    ldi r24, lo8(.LC0)
    ldi r25, hi8(.LC0)
    ; .LC0: "Starting"
    ; call Serial.println(value)
    call Serial_println

; Expression 2: Arduino call statement
; Result convention: r24:r25 holds the final value
    ; load integer literal 1000
    ldi r24, lo8(1000)
    ldi r25, hi8(1000)
    ; call delay(milliseconds)
    call delay


; ===== Function loop =====
; Expression 3: Arduino call statement
; Result convention: r24:r25 holds the final value
    ; load address of string literal "Looping"
    ldi r24, lo8(.LC1)
    ldi r25, hi8(.LC1)
    ; .LC1: "Looping"
    ; call Serial.print(value)
    call Serial_print

; Expression 4: Arduino call statement
; Result convention: r24:r25 holds the final value
    ; load integer literal 500
    ldi r24, lo8(500)
    ldi r25, hi8(500)
    ; call delay(milliseconds)
    call delay

## test3_if_else.c

; Generated from ../Parser/ast.txt
; Expression Code Generation (AVR-style)

; ===== Function setup =====
; Expression 1: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load integer literal 3
    ldi r24, lo8(3)
    ldi r25, hi8(3)

; Expression 2: if condition
; Result convention: r24:r25 holds the final value
    ; load variable x
    lds r24, x
    lds r25, x+1
    push r25
    push r24
    ; load integer literal 5
    ldi r24, lo8(5)
    ldi r25, hi8(5)
    pop r18
    pop r19
    ; compare left < right
    cp r18, r24
    cpc r19, r25
    brlt .Lexpr_true_0
    clr r24
    clr r25
    rjmp .Lexpr_end_1
.Lexpr_true_0:
    ldi r24, 1
    clr r25
.Lexpr_end_1:

; Expression 3: assignment value
; Result convention: r24:r25 holds the final value
    ; load variable x
    lds r24, x
    lds r25, x+1
    push r25
    push r24
    ; load integer literal 1
    ldi r24, lo8(1)
    ldi r25, hi8(1)
    pop r18
    pop r19
    ; add left operand (r19:r18) to right operand (r25:r24)
    add r24, r18
    adc r25, r19

; Expression 4: assignment value
; Result convention: r24:r25 holds the final value
    ; load variable x
    lds r24, x
    lds r25, x+1
    push r25
    push r24
    ; load integer literal 1
    ldi r24, lo8(1)
    ldi r25, hi8(1)
    pop r18
    pop r19
    ; subtract right operand from left operand
    sub r18, r24
    sbc r19, r25
    mov r24, r18
    mov r25, r19


; ===== Function loop =====
; Expression 5: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load boolean literal false
    ldi r24, 0
    clr r25

; Expression 6: if condition
; Result convention: r24:r25 holds the final value
    ; load variable flag
    lds r24, flag
    lds r25, flag+1
    ; logical not
    or r24, r25
    breq .Lexpr_true_6
    clr r24
    clr r25
    rjmp .Lexpr_end_7
.Lexpr_true_6:
    ldi r24, 1
    clr r25
.Lexpr_end_7:

; Expression 7: assignment value
; Result convention: r24:r25 holds the final value
    ; load boolean literal true
    ldi r24, 1
    clr r25

; Expression 8: assignment value
; Result convention: r24:r25 holds the final value
    ; load boolean literal false
    ldi r24, 0
    clr r25

## test4_semantic_ok.c

; Generated from ../Parser/ast.txt
; Expression Code Generation (AVR-style)

; ===== Function setup =====
; Expression 1: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load integer literal 0
    ldi r24, lo8(0)
    ldi r25, hi8(0)
    ; call analogRead(pin)
    call analogRead

; Expression 2: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load boolean literal true
    ldi r24, 1
    clr r25

; Expression 3: if condition
; Result convention: r24:r25 holds the final value
    ; load variable sensor
    lds r24, sensor
    lds r25, sensor+1
    push r25
    push r24
    ; load integer literal 10
    ldi r24, lo8(10)
    ldi r25, hi8(10)
    pop r18
    pop r19
    ; compare left > right
    cp r18, r24
    cpc r19, r25
    brlt .Lexpr_false_2
    breq .Lexpr_false_2
    ldi r24, 1
    clr r25
    rjmp .Lexpr_end_3
.Lexpr_false_2:
    clr r24
    clr r25
.Lexpr_end_3:
    push r25
    push r24
    ; load variable ready
    lds r24, ready
    lds r25, ready+1
    pop r18
    pop r19
    ; logical and
    or r18, r19
    breq .Lexpr_false_0
    or r24, r25
    breq .Lexpr_false_0
    ldi r24, 1
    clr r25
    rjmp .Lexpr_end_1
.Lexpr_false_0:
    clr r24
    clr r25
.Lexpr_end_1:

; Expression 4: Arduino call statement
; Result convention: r24:r25 holds the final value
    ; load address of string literal "sensor ready"
    ldi r24, lo8(.LC0)
    ldi r25, hi8(.LC0)
    ; .LC0: "sensor ready"
    ; call Serial.println(value)
    call Serial_println


; ===== Function loop =====
; Expression 5: assignment value
; Result convention: r24:r25 holds the final value
    ; load integer literal 2
    ldi r24, lo8(2)
    ldi r25, hi8(2)
    ; call digitalRead(pin)
    call digitalRead
    push r25
    push r24
    ; load integer literal 1
    ldi r24, lo8(1)
    ldi r25, hi8(1)
    pop r18
    pop r19
    ; add left operand (r19:r18) to right operand (r25:r24)
    add r24, r18
    adc r25, r19

; Expression 6: while condition
; Result convention: r24:r25 holds the final value
    ; load variable value
    lds r24, value
    lds r25, value+1
    push r25
    push r24
    ; load integer literal 4
    ldi r24, lo8(4)
    ldi r25, hi8(4)
    pop r18
    pop r19
    ; compare left < right
    cp r18, r24
    cpc r19, r25
    brlt .Lexpr_true_6
    clr r24
    clr r25
    rjmp .Lexpr_end_7
.Lexpr_true_6:
    ldi r24, 1
    clr r25
.Lexpr_end_7:

; Expression 7: assignment value
; Result convention: r24:r25 holds the final value
    ; load variable value
    lds r24, value
    lds r25, value+1
    push r25
    push r24
    ; load integer literal 1
    ldi r24, lo8(1)
    ldi r25, hi8(1)
    pop r18
    pop r19
    ; add left operand (r19:r18) to right operand (r25:r24)
    add r24, r18
    adc r25, r19