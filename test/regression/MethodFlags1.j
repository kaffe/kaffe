
.class MethodFlags1
.super java/lang/Object

.method abstract final main([Ljava/lang/String;)V
.end method

; No stack trace
; Expected Output:
; java.lang.ClassFormatError: (class: MethodFlags1, method: main signature: ([Ljava/lang/String;)V) Abstract is incompatible with final, native, private, static, strict, and synchronized
