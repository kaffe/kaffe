
.class public InvokedMethodSignature1
.super  java/lang/Object

.method public static main([Ljava/lang/String;)V
    .limit stack 1
    .limit locals 1
    invokestatic java/lang/ClassLoader/getSystemClassLoader(M)Ljava/lang/ClassLoader;
    pop
    return
.end method

; No stack trace
; Expected Output:
; java.lang.ClassFormatError: InvokedMethodSignature1 (Method "getSystemClassLoader" has illegal signature "(M)Ljava/lang/ClassLoader;")
