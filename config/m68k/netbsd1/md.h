/*
 * m68k/netbsd1/md.h
 * netbsd/m68k configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * Written by Kiyo Inaba <inaba@src.ricoh.co.jp>, 1998;
 */

#ifndef __m68k_netbsd1_md_h
#define __m68k_netbsd1_md_h

#include "m68k/common.h"
#include "m68k/threads.h"

/*
 * Alignment in structure is 8 bytes packed
 */
#define ALIGNMENT_OF_SIZE(S)    ((S) < 8 ? (S) : 8)

/*
 * Stack offset.
 * This is the offset into the setjmp buffer where the stack pointer is
 * stored.
 */
#define SP_OFFSET       2

/**/
/* Extra exception handling information. */
/**/
#include <signal.h>

#define	SIGNAL_ARGS(sig, sc) int sig, int code, struct sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext *scp
#define GET_SIGNAL_CONTEXT_POINTER(scp) (scp)
#define SIGNAL_PC(scp) ((uintp)(scp)->sc_pc)

#include "kaffe-unix-stack.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/*
 * sysdepCallMethod supports:
 *
 *	NetBSD version
 *	For NetBSD the 'extraargs' magic does not work, and I still
 *	have to use fully asm version. Also, with optimization A2
 *	register is contaminated when coming back from subroutine
 *	sysdepCallMethod calls, and I split the code into two parts.
 */

#define	sysdepCallMethod(CALL)					  \
	asm volatile ("						\n\
	1:							\n\
		cmp.l	#0,%0					\n\
		beq	3f					\n\
		subq.l	#1,%0					\n\
		cmp.b	#0,(%2,%0.l)				\n\
		beq	1b					\n\
		cmp.b	#1,(%2,%0.l)				\n\
		beq	2f					\n\
		move.l	4(%1,%0.l*8),-(%/sp)			\n\
	2:							\n\
		move.l	(%1,%0.l*8),-(%/sp)			\n\
		jmp	1b					\n\
	3:							\n\
		jsr	(%3)					\n\
      " :							  \
	: "r" ((CALL)->nrargs),					  \
	  "a" ((CALL)->args),					  \
	  "a" ((CALL)->callsize),				  \
	  "a" ((CALL)->function)				  \
	: "d0", "d1", "fp0", "cc", "memory");			  \
	asm volatile ("						\n\
		cmp.b	#2,%0					\n\
		bne	5f					\n\
		cmp.b	#0x44,%1				\n\
		bne	4f					\n\
		fmove.d	%/fp0,(%2)				\n\
		jmp	7f					\n\
	4:							\n\
		move.l	%/d1,4(%2)				\n\
		move.l	%/d0,(%2)				\n\
		jmp	7f					\n\
	5:							\n\
		cmp.b	#1,%0					\n\
		bne	7f					\n\
		cmp.b	#0x46,%1				\n\
		bne	6f					\n\
		fmove.s	%/fp0,(%2)				\n\
		jmp	7f					\n\
	6:							\n\
		move.l	%/d0,(%2)				\n\
	7:							  \
      " :							  \
	: "m" ((CALL)->retsize),				  \
	  "m" ((CALL)->rettype),				  \
	  "a" ((CALL)->ret)					  \
	: "d0", "d1", "fp0", "cc", "memory");			  \
	asm volatile ("						\n\
		add.w %0,%/sp					\n\
	" : : "r" ((CALL)->argsize * sizeof(jint)) : "cc")

#endif /* __m68k_netbsd1_md_h */
