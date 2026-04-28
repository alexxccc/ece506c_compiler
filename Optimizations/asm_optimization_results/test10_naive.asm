; Generated from ../Parser/ast.txt
; Expression Code Generation (AVR-style)
; Enabled optimizations: none

; ===== Function setup =====
; Expression 1: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load variable input
    lds r24, input
    lds r25, input+1
    push r25
    push r24
    ; load integer literal 0
    ldi r24, lo8(0)
    ldi r25, hi8(0)
    pop r18
    pop r19
    ; multiply 16-bit operands using runtime helper
    mov r22, r18
    mov r23, r19
    call __mulhi3

; Expression 2: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load variable input
    lds r24, input
    lds r25, input+1
    push r25
    push r24
    ; load integer literal 1
    ldi r24, lo8(1)
    ldi r25, hi8(1)
    pop r18
    pop r19
    ; multiply 16-bit operands using runtime helper
    mov r22, r18
    mov r23, r19
    call __mulhi3

; Expression 3: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load variable input
    lds r24, input
    lds r25, input+1
    push r25
    push r24
    ; load integer literal 0
    ldi r24, lo8(0)
    ldi r25, hi8(0)
    pop r18
    pop r19
    ; add left operand (r19:r18) to right operand (r25:r24)
    add r24, r18
    adc r25, r19

; Expression 4: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load variable input
    lds r24, input
    lds r25, input+1
    push r25
    push r24
    ; load integer literal 0
    ldi r24, lo8(0)
    ldi r25, hi8(0)
    pop r18
    pop r19
    ; subtract right operand from left operand
    sub r18, r24
    sbc r19, r25
    mov r24, r18
    mov r25, r19

; Expression 5: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load variable input
    lds r24, input
    lds r25, input+1
    push r25
    push r24
    ; load integer literal 1
    ldi r24, lo8(1)
    ldi r25, hi8(1)
    pop r18
    pop r19
    ; divide 16-bit operands using runtime helper
    mov r22, r18
    mov r23, r19
    call __divhi3


; ===== Function loop =====
