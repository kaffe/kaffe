/*
 * alpha/common.h
 * Common Alpha configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __alpha_common_h
#define __alpha_common_h

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

#endif
