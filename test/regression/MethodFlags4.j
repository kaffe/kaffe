
.class public abstract MethodFlags4
.super java/lang/Object

.method public abstract main([Ljava/lang/String;)V
    .limit stack 2
    
    getstatic java/lang/System/out Ljava/io/PrintStream;
    
    ldc "Hello World!"
    
    invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
    
    return
.end method

; No stack trace
; Expected Output:
; java.lang.ClassFormatError: (class: MethodFlags4, method: main, signature: ([Ljava/lang/String;)V) Native or abstract method cannot have a code attribute
