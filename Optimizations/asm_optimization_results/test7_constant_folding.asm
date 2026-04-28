; Generated from ../Parser/ast.txt
; Expression Code Generation (AVR-style)
; Enabled optimizations: constant-folding

; ===== Function setup =====
; Expression 1: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load integer literal 5
    ldi r24, lo8(5)
    ldi r25, hi8(5)


; ===== Function loop =====
; Expression 2: assignment value
; Result convention: r24:r25 holds the final value
    ; load integer literal 20
    ldi r24, lo8(20)
    ldi r25, hi8(20)

