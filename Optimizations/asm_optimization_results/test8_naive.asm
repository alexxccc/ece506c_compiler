; Generated from ../Parser/ast.txt
; Expression Code Generation (AVR-style)
; Enabled optimizations: none

; ===== Function setup =====
; Expression 1: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load integer literal 7
    ldi r24, lo8(7)
    ldi r25, hi8(7)

; Expression 2: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load variable known
    lds r24, known
    lds r25, known+1


; ===== Function loop =====
; Expression 3: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load integer literal 2
    ldi r24, lo8(2)
    ldi r25, hi8(2)

; Expression 4: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load variable base
    lds r24, base
    lds r25, base+1
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

