
.class public FieldRefSignature1
.super  java/lang/Object

.field a I

.method method()I
    .limit stack 1
    .limit locals 1
    aload_0
    getfield InvokedMethodSignature1/a M
    freturn
.end method

; No stack trace
; Expected Output:
; java.lang.ClassFormatError: FieldRefSignature1 (Field "a" has illegal signature "M")
