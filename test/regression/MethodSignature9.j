
.class abstract MethodSignature9
.super java/lang/Object

.method abstract foo()V
.end method

.method abstract foo()V
.end method

; No stack trace
; Expected Output:
; java.lang.ClassFormatError: (class: MethodSignature9, method: foo signature: ()V) Duplicate method.
