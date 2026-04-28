; Generated from ../Parser/ast.txt
; Expression Code Generation (AVR-style)
; Enabled optimizations: dead-code-elimination

; ===== Function setup =====
; Expression 1: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load integer literal 1
    ldi r24, lo8(1)
    ldi r25, hi8(1)

; Expression 2: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load integer literal 3
    ldi r24, lo8(3)
    ldi r25, hi8(3)


; ===== Function loop =====
