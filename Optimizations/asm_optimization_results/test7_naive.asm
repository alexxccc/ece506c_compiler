; Generated from ../Parser/ast.txt
; Expression Code Generation (AVR-style)
; Enabled optimizations: none

; ===== Function setup =====
; Expression 1: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load integer literal 2
    ldi r24, lo8(2)
    ldi r25, hi8(2)
    push r25
    push r24
    ; load integer literal 3
    ldi r24, lo8(3)
    ldi r25, hi8(3)
    pop r18
    pop r19
    ; add left operand (r19:r18) to right operand (r25:r24)
    add r24, r18
    adc r25, r19


; ===== Function loop =====
; Expression 2: assignment value
; Result convention: r24:r25 holds the final value
    ; load integer literal 4
    ldi r24, lo8(4)
    ldi r25, hi8(4)
    push r25
    push r24
    ; load integer literal 5
    ldi r24, lo8(5)
    ldi r25, hi8(5)
    pop r18
    pop r19
    ; multiply 16-bit operands using runtime helper
    mov r22, r18
    mov r23, r19
    call __mulhi3

