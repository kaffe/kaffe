
.class  BadCatch2
.super  java/lang/Throwable

.method public static main([Ljava/lang/String;)V

    .limit stack 3
    .limit locals 2
.line 8
.line 10
.catch BadCatch2 from Label2 to Label1 using Label2
Label1:
    new BadCatch2
    dup
    invokespecial BadCatch2/<init>()V
    athrow
Label2:
    astore_1
.line 14
    getstatic java/lang/System/out Ljava/io/PrintStream;
    ldc "Caught"
    invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
.line 16
    return
.end method

.method <init>()V
    .limit stack 1
    .limit locals 1
.line 2
    aload_0
    invokespecial java/lang/Throwable/<init>()V
.line 2
    return
.end method

; No line numbers
; Expected Output:
; java.lang.VerifyError: (class: BadCatch2, method: main signature: ([Ljava/lang/String;)V) Illegal exception end pc: 0
;	at java.lang.reflect.Method.invoke0(Method.java:XXXXnative)
;	at java.lang.reflect.Method.invoke(Method.java:XXXX)
;	at DieAfter.main(DieAfter.java:XXXX)

; JanosVM Expected Output:
; java.lang.VerifyError: (class: BadCatch2, method: main signature: ([Ljava/lang/String;)V) Illegal exception end pc: 0
