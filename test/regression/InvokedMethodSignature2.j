
.class public InvokedMethodSignature2
.super  java/lang/Object

.method public static foo([Ljava/lang/String;)V
    .limit stack 1
    .limit locals 1
    invokespecial java/lang/ClassLoader/<foo>()V
    pop
    return
.end method

; No stack trace
; Expected Output:
; java.lang.VerifyError: (class: InvokedMethodSignature2, method: <foo> signature: ()V) Illegal call to internal method
