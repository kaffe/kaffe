/*
 * i386/common.h
 * Common i386 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_h
#define __i386_h

/* The 386 never aligns to more than a 4 byte boundary. */
#define	ALIGNMENT_OF_SIZE(S)	((S) < 4 ? (S) : 4)

/*
 * Make a call to a native or Java (JIT) method.
 *  This assembly code should build a standard C call using the passed
 *  call information.  By its nature this is highly processor specific.
 *  This function is mandatory for both JIT and Interpreter (since stubs
 *  have now been deprecated).
 */
#define	sysdepCallMethod(CALL)						\
	asm volatile ("							\n\
1:									\n\
		cmpl $0,%0						\n\
		je 3f							\n\
		decl %0							\n\
		cmpb $0,(%2,%0)						\n\
		je 1b							\n\
		cmpb $1,(%2,%0)						\n\
		je 2f							\n\
		pushl 4(%1,%0,8)					\n\
2:									\n\
		pushl (%1,%0,8)						\n\
		jmp 1b							\n\
3:									\n\
		call *%3						\n\
		movl %5,%%ebx						\n\
		movb %4,%%cl						\n\
		cmpb $0x46,%%cl						\n\
		jne 4f							\n\
		fstps (%%ebx)						\n\
		jmp 6f							\n\
4:									\n\
		cmpb $0x44,%%cl						\n\
		jne 5f							\n\
		fstpl (%%ebx)						\n\
		jmp 6f							\n\
5:									\n\
		movl %%eax,(%%ebx)					\n\
		cmpb $0x4a,%%cl						\n\
		jne 6f							\n\
		movl %%edx,4(%%ebx)					\n\
6:									\n\
	" :								\
	  : "r" ((CALL)->nrargs),					\
	    "r" ((CALL)->args),						\
	    "r" ((CALL)->callsize),					\
	    "m" ((CALL)->function),					\
	    "m" ((CALL)->rettype),					\
	    "m" ((CALL)->ret)						\
	  : "eax", "ebx", "ecx", "edx", "edi", "esi", "cc", "memory");	\
	asm volatile ("							\n\
		subl %0,%%esp						\n\
	" : : "r" ((CALL)->argsize * sizeof(jint)) : "cc")

#endif
