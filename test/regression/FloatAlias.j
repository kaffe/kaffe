; FloatAlias is used to test for bad slot aliasing on x86.

.class  FloatAlias
.super  java/lang/Object

.method public static main([Ljava/lang/String;)V
    .limit stack 2
    .limit locals 2
    ldc 10.0
    ; BEGIN Important part
    fstore_1
    fload_1
    ; END Important part
    ldc 7.0
    fcmpg
    ifge Label1
    ldc 7.0
    fstore_1
Label1:
    getstatic java/lang/System/out Ljava/io/PrintStream;
    fload_1
    invokevirtual java/io/PrintStream/println(F)V
    return
.end method

; Expected Output:
; 10.0
