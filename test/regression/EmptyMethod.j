
.class public EmptyMethod
.super java/lang/Object

.method public static foo([Ljava/lang/String;)V
.end method

; No stack trace
; Expected Output:
; java.lang.VerifyError: (class: EmptyMethod, method: foo signature: ([Ljava/lang/String;)V) Empty code
