/*
 * i386/qnx/md.h
 * QNX i386 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_qnx_md_h
#define __i386_qnx_md_h

#if !defined(HAVE_GETPAGESIZE)
#undef getpagesize()
#define cdecl
#include <sys/mman.h>
#define       getpagesize()   PAGESIZE
#endif

#include "i386/common.h"
#include "i386/threads.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* QNX requires a little initialisation */
#define       INIT_MD()       init_md()

/*
 * QNX doesn't do function calls in the "standard" way, specifically it
 * returns floats and doubles in registers rather than in the floating
 * point unit.  We must make some changes to the sysdepCallMethod to
 * handle this.
 */
#undef	sysdepCallMethod
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
		jmpl 1b							\n\
3:									\n\
		call *%3						\n\
		movl %5,%%ebx						\n\
		movb %4,%%cl						\n\
		movl %%eax,(%%ebx)					\n\
		cmpb $'D',%%cl						\n\
		je 4f							\n\
		cmpb $'J',%%cl						\n\
		jne 5f							\n\
4:									\n\
		movl %%edx,4(%%ebx)					\n\
5:									\n\
	" :								\
	  : "r" ((CALL)->nrargs),					\
	    "r" ((CALL)->args),						\
	    "r" ((CALL)->callsize),					\
	    "m" (METHOD_NATIVECODE((CALL)->meth)),			\
	    "m" ((CALL)->rettype),					\
	    "m" ((CALL)->ret)						\
	  : "eax", "ebx", "ecx", "edx", "edi", "esi", "cc", "memory");	\
	asm volatile ("							\n\
		subl %0,%%esp						\n\
	" : : "r" ((CALL)->argsize * sizeof(jint)) : "cc")

#endif
