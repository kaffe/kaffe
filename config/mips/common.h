/*
 * mips/common.h
 * Common MIPS configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __mips_common_h
#define __mips_common_h

/*
 * Make a call to a native or Java (JIT) method.
 *  This assembly code should build a standard C call using the passed
 *  call information.  By its nature this is highly processor specific.
 *  This function is mandatory for the JIT, but can be avoided for
 *  the interpreter if you don't define NO_KAFFE_STUBS.  However, the
 *  Kaffe stubs will be deprecated at some time in the future and
 *  this function will be needed for JNI support when it comes.
 */
#define	NO_KAFFE_STUBS
#define	sysdepCallMethod(CALL)						\
	asm volatile ("							\n\
	" :								\
	  : "r" ((CALL)->nrargs),					\
	    "r" ((CALL)->args),						\
	    "r" ((CALL)->callsize),					\
	    "m" ((CALL)->function),					\
	    "m" ((CALL)->rettype),					\
	    "m" ((CALL)->ret)						\
	  : "eax", "ebx", "ecx", "edx", "edi", "esi", "cc", "memory");	\
	asm volatile ("							\n\
	" : : "r" ((CALL)->argsize * sizeof(jint)) : "cc")

#define	CALL_KAFFE_FUNCTION_VARARGS(meth, obj, nargs, argptr, retval)	\
	asm volatile ("							\n\
		addiu $sp, $sp, -96 					\n\
		sw $16, 8($sp)						\n\
		sw $17, 12($sp)						\n\
		sw $18, 16($sp)						\n\
		sw $19, 20($sp)						\n\
		sw $20, 24($sp)						\n\
		sw $21, 28($sp)						\n\
		sw $22, 32($sp)						\n\
		sw $23, 36($sp)						\n\
		swc1 $f20, 40($sp)					\n\
		swc1 $f21, 44($sp)					\n\
		swc1 $f22, 48($sp)					\n\
		swc1 $f23, 52($sp)					\n\
		swc1 $f24, 56($sp)					\n\
		swc1 $f25, 60($sp)					\n\
		swc1 $f26, 64($sp)					\n\
		swc1 $f27, 68($sp)					\n\
		swc1 $f28, 72($sp)					\n\
		swc1 $f29, 76($sp)					\n\
		swc1 $f30, 80($sp)					\n\
		swc1 $f31, 84($sp)					\n\
		lw $5, 0(%2)						\n\
		lw $6, 4(%2)						\n\
		lw $7, 8(%2)						\n\
                move $4, %0                                             \n\
                move $25, %1                                            \n\
                jal $25                                                 \n\
		lwc1 $f31, 84($sp)					\n\
		lwc1 $f30, 80($sp)					\n\
		lwc1 $f29, 76($sp)					\n\
		lwc1 $f28, 72($sp)					\n\
		lwc1 $f27, 68($sp)					\n\
		lwc1 $f26, 64($sp)					\n\
		lwc1 $f25, 60($sp)					\n\
		lwc1 $f24, 56($sp)					\n\
		lwc1 $f23, 52($sp)					\n\
		lwc1 $f22, 48($sp)					\n\
		lwc1 $f21, 44($sp)					\n\
		lwc1 $f20, 40($sp)					\n\
		lw $23, 8($sp)						\n\
		lw $22, 12($sp)						\n\
		lw $21, 16($sp)						\n\
		lw $20, 20($sp)						\n\
		lw $19, 24($sp)						\n\
		lw $18, 28($sp)						\n\
		lw $17, 32($sp)						\n\
		lw $16, 36($sp)						\n\
                addiu $sp, $sp, 96 					\n\
	" : : "r" (obj), "r" (meth->ncode), "r" (argptr) :		\
		"$4", "$5", "$6", "$7", "$25")

#endif
