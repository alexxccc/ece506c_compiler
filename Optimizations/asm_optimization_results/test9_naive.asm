; Generated from ../Parser/ast.txt
; Expression Code Generation (AVR-style)
; Enabled optimizations: none

; ===== Function setup =====
; Expression 1: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load integer literal 1
    ldi r24, lo8(1)
    ldi r25, hi8(1)

; Expression 2: if condition
; Result convention: r24:r25 holds the final value
    ; load boolean literal false
    ldi r24, 0
    clr r25

; Expression 3: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load integer literal 2
    ldi r24, lo8(2)
    ldi r25, hi8(2)

; Expression 4: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load integer literal 3
    ldi r24, lo8(3)
    ldi r25, hi8(3)

; Expression 5: while condition
; Result convention: r24:r25 holds the final value
    ; load boolean literal false
    ldi r24, 0
    clr r25

; Expression 6: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load integer literal 4
    ldi r24, lo8(4)
    ldi r25, hi8(4)

; Expression 7: declaration initializer
; Result convention: r24:r25 holds the final value
    ; load integer literal 5
    ldi r24, lo8(5)
    ldi r25, hi8(5)


; ===== Function loop =====
