/*
 * m68k/common.h
 * Common M68K configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __m68k_common_h
#define __m68k_common_h

#if 0
#define	NO_KAFFE_STUBS
#define	sysdepCallMethod(CALL)						\
	asm volatile ("							\n\
		ld [%2],%%o0						\n\
		ld [%2+4],%%o1						\n\
		ld [%2+8],%%o2						\n\
		ld [%2+12],%%o3						\n\
		ld [%2+16],%%o4						\n\
		ld [%2+20],%%o5						\n\
		call %1,0						\n\
		nop							\n\
		st [%0],%%o0						\n\
	" :								\
	  : "r" ((CALL)->ret),						\
	    "r" ((CALL)->function),					\
	    "r" ((CALL)->args)						\
	  : "g1" , "g2", "g3", "g4", "o0", "o1", "o2", "o3", "o4", "o5", "o7" )

#define	CALL_KAFFE_METHOD_VARARGS(meth, obj, nargs, argptr, retval)	\

	movem%.l %/d0-%/d7/%/a0-%/a5,%-
	fmovem %/fp0-%/fp7,%-

	move%.l %3,%/d0
	beq 2f
	lea 0(%4,%/d0:l*4),%/a0
	subq %#1,%/d0
1:	move%.l -(%/a0),%-
	dbra %/d0, 1b

2:	move%.l %2,%-
	jsr (%1)
	move%.l %5,%/d1
	addl %/d1,%/sp
	move%.l %/d0,%0
	fmovem %+,%/fp0-%/fp7
	movem%.l %+,%/d0-%/d7/%/a0-%/a5

	: "=m"(retval)						\
	: "a"(meth->ncode), "g"(obj), "g"(nargs), "a"(argptr),	\
	  "m"(4*(nargs+1))					\
	: "cc", "memory")

#endif

#endif
