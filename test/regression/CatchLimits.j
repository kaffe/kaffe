; CatchLimits.j
; Copyright (c) 2002 Patrick Tullmann <pat@tullmann.org>
; All Rights Reserved.
;
; Based on Jasmin's "examples/Catch.j" by Jonathan Meyer:
;  ; --- Copyright Jonathan Meyer 1996. All rights reserved. -----------------
;  ; File:      jasmin/examples/Catch.j
;  ; Author:    Jonathan Meyer, 10 July 1996
; Jasmin is licensed under the GPL.
;

;
; Test the limits of catch ranges.  Throw exceptions before and after
; the start/end boundaries of catch ranges.
;
; This is somewhat complicated because it returns an exit code to
; indicate failure (vs. just printing something unexpected), because
; TestScript.in only looks at the return code for saved .class files.
;
; NOTE:
; The file is *NOT* automatically recompiled.  You need the Jasmin
; bytecode compiler, and you'll need to copy the generated CatchLimits.class
; file to CatchLimits.class.save.
;

;
.class public CatchLimits
.super java/lang/Object

; Field for tracking the return code.  Starts at zero, each failure increments it
.field private static exitCode I = 0


;; --- standard initializer
.method public <init>()V
	aload_0
	invokenonvirtual java/lang/Object.<init>()V
	return
.end method


;; --- Increment the exitCode field to indicate that an error occured
.method public static errorOccured()V
	.limit locals 0
	.limit stack 3
	getstatic CatchLimits/exitCode I
	bipush 1	
	iadd 		; 1 + exitCode
	putstatic CatchLimits/exitCode I
	return
.end method


;; --- Test with a single (throw) instruction in the catch boundaries
.method private static onlyCatch()V
	.limit locals 2
	.limit stack 5

	.catch java/lang/Exception from OnlyCatchL1 to OnlyCatchL2 using OnlyCatch

	; store System.out in local variable 1
	getstatic java/lang/System/out Ljava/io/PrintStream;
	astore_1

	; Create an exception
	new java/lang/Exception
	dup
	ldc "this should be caught"
	invokenonvirtual java/lang/Exception/<init>(Ljava/lang/String;)V

OnlyCatchL1:
	; Throw the exception.  This is the *only* instruction within
	; the catch boundary.
	athrow
OnlyCatchL2:
OnlyCatch:
	aload_1
	ldc "SUCCESS: Caught only-instruction exception."
	invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
	return
.end method


;; --- Test with a throw as first of many instructions in catch range
.method private static headCatch()V
	.limit locals 3
	.limit stack 5

	.catch java/lang/Exception from HeadCatchL1 to HeadCatchL2 using HeadCatch

	; store System.out in local variable 1
	getstatic java/lang/System/out Ljava/io/PrintStream;
	astore_1

	; Create an exception
	new java/lang/Exception
	dup
	ldc "this should be caught"
	invokenonvirtual java/lang/Exception/<init>(Ljava/lang/String;)V

HeadCatchL1:
	; Throw the exception.
	athrow
	; None of the following will execute, but they exist to
	; bloat the catch range
	new java/lang/Exception
	dup
	ldc "this should be caught"
	invokenonvirtual java/lang/Exception/<init>(Ljava/lang/String;)V
HeadCatchL2:
HeadCatch:
	aload_1
	ldc "SUCCESS: Caught head-of-range exception."
	invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
	return
.end method


;; --- Test with a throw as last of many instructions in catch range
.method private static tailCatch()V
	.limit locals 3
	.limit stack 5

	.catch java/lang/Exception from TailCatchL1 to TailCatchL2 using TailCatch

	; store System.out in local variable 1
	getstatic java/lang/System/out Ljava/io/PrintStream;
	astore_1

TailCatchL1:
	; Create an exception (inside the catch range)
	new java/lang/Exception
	dup
	ldc "this should be caught"
	invokenonvirtual java/lang/Exception/<init>(Ljava/lang/String;)V

	; Throw the exception.  This is the last instruction in the
	; catch range
	athrow
TailCatchL2:
TailCatch:
	aload_1
	ldc "SUCCESS: Caught tail-of-range exception."
	invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
	return
.end method


;; --- Test with a throw before the catch range
.method private static headMiss()V
	.limit locals 3
	.limit stack 5

	.catch java/lang/Exception from HeadMissL1 to HeadMissL2 using HeadMiss

	; store System.out in local variable 1
	getstatic java/lang/System/out Ljava/io/PrintStream;
	astore_1

	; Create an exception
	new java/lang/Exception
	dup
	ldc "this should be caught"
	invokenonvirtual java/lang/Exception/<init>(Ljava/lang/String;)V

	; Throw the exception (just *before* the catch range)
	athrow
HeadMissL1:
HeadMiss:
	; Put the handler inside the exception block so there is 
	; something useful (and reachable) inside the block
	aload_1
	ldc "FAILURE: Caught exception thrown before catch range."
	invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
	invokestatic CatchLimits/errorOccured()V
HeadMissL2:
	return
.end method


;; --- Test with throw just after the catch range
.method private static tailMiss()V
	.limit locals 3
	.limit stack 5

	.catch java/lang/Exception from TailMissL1 to TailMissL2 using TailMiss

	; store System.out in local variable 1
	getstatic java/lang/System/out Ljava/io/PrintStream;
	astore_1

TailMissL1:
	; Create an exception
	new java/lang/Exception
	dup
	ldc "this should be caught"
	invokenonvirtual java/lang/Exception/<init>(Ljava/lang/String;)V

TailMissL2:
	; Throw the exception (just *after* the catch range)
	athrow
TailMiss:
	aload_1
	ldc "FAILURE: Caught exception thrown after catch range."
	invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
	invokestatic CatchLimits/errorOccured()V
	return
.end method



;; --- MAIN.  Run each of the above test functions
.method public static main([Ljava/lang/String;)V
	.limit locals 3
	.limit stack 5
	
	; Handlers for each test case; first three shouldn't catch
	; last two should
	.catch java/lang/Exception from T1start to T1end using T1handle
	.catch java/lang/Exception from T2start to T2end using T2handle
	.catch java/lang/Exception from T3start to T3end using T3handle
	.catch java/lang/Exception from T4start to T4end using T4handle
	.catch java/lang/Exception from T5start to T5end using T5handle
	
	; store System.out in local variable 1
	getstatic java/lang/System/out Ljava/io/PrintStream;
	astore_1
	
	; print out a message
	aload_1
	ldc "Testing throw at limit of catch range..."
	invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V

T1start:
	nop
	nop
	invokestatic CatchLimits/onlyCatch()V
	goto T2start
	nop
	nop
T1end:
T1handle:
	pop	; pop the pushed exception
	aload_1
	ldc "FAILURE: throw as only instruction in catch range failed."
	invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
	invokestatic CatchLimits/errorOccured()V


T2start:
	nop
	nop
	invokestatic CatchLimits/headCatch()V
	goto T3start
	nop
	nop
T2end:
T2handle:
	pop	; pop the pushed exception
	aload_1
	ldc "FAILURE: throw as head instruction in catch range failed."
	invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
	invokestatic CatchLimits/errorOccured()V


T3start:
	nop
	nop
	invokestatic CatchLimits/tailCatch()V
	goto T4start
	nop
	nop
T3end:
T3handle:
	pop	; pop the pushed exception
	aload_1
	ldc "FAILURE: throw as tail instruction in catch range failed."
	invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
	invokestatic CatchLimits/errorOccured()V


T4start:
	nop
	nop
	invokestatic CatchLimits/headMiss()V
	goto T5start
	nop
	nop
T4end:
T4handle:
	pop	; pop the pushed exception
	aload_1
	ldc "SUCCESS: throw before catch range"
	invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V


T5start:
	nop
	nop
	invokestatic CatchLimits/tailMiss()V
	goto T6start
	nop
	nop
T5end:
T5handle:
	pop	; pop the pushed exception
	aload_1
	ldc "SUCCESS: throw after catch range"
	invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V

T6start:

	;; invoke System.exit(exitCode)
	getstatic CatchLimits/exitCode I
	invokestatic java/lang/System/exit(I)V

	;; exit doesn't return, but the verifier don't know that...

	return

.end method ; CatchLimits.main()
