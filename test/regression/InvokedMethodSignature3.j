
.class public InvokedMethodSignature3
.super  java/lang/Object

.method public static foo([Ljava/lang/String;)V
    .limit stack 1
    .limit locals 1
    invokespecial java/lang/ClassLoader/<init>()I
    pop
    return
.end method

; No stack trace
; Expected Output:
; java.lang.VerifyError: InvokedMethodSignature3 (Method "<init>" has illegal signature "()I")
