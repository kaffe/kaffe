/*
 * m68k/netbsd/md.h
 * netbsd/m68k specific functions.
 *
 * Copyright (c) 1996 Matthias Hopf <mshopf@informatik.uni-erlangen.de>
 * netbsd/m68k version  Erik Johannessen <erjohann@sn.no>
 *
 * Permission granted for Tim Wilkinson to include this source in his
 * Kaffe system.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __m68k_openbsd_md_h
#define __m68k_openbsd_md_h

#include "m68k/common.h"
#include "m68k/threads.h"

/*
 * Stack offset.
 * This is the offset into the setjmp buffer 
 * where the stack pointer is stored.
 */
#undef	SP_OFFSET
#define	SP_OFFSET		2

/**/
/* Extra exception handling information. */
/**/
#include <signal.h>

#define	SIGNAL_ARGS(sig, sc) int sig, siginfo_t* code, struct sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext *scp
#define GET_SIGNAL_CONTEXT_POINTER(scp) (scp)
#define SIGNAL_PC(scp) ((uintp)(scp)->sc_pc)
#define STACK_STRUCT struct sigaltstack

#include "kaffe-unix-stack.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#define	sysdepCallMethod(CALL)					  \
	asm volatile (						\
	"1:							\n"\
	"	cmpl	#0,%0					\n"\
	"	beq	3f					\n"\
	"	subql	#1,%0					\n"\
	"	cmpb	#0,%2@(%0:l)				\n"\
	"	beq	1b					\n"\
	"	cmpb	#1,%2@(%0:l)				\n"\
	"	beq	2f					\n"\
	"	movel	%1@(4,%0:l:8),sp@-			\n"\
	"2:							\n"\
	"	movel	%1@(%0:l:8),sp@-			\n"\
	"	jmp	1b					\n"\
	"3:							\n"\
	"	jsr	%3@					\n"\
        :							  \
	: "r" ((CALL)->nrargs),					  \
	  "a" ((CALL)->args),					  \
	  "a" ((CALL)->callsize),				  \
	  "a" ((CALL)->function)				  \
	: "d0", "d1", "fp0", "cc", "memory");			  \
	asm volatile (						\
	"	cmpb	#2,%0					\n"\
	"	bne	5f					\n"\
	"	cmpb	#0x44,%1				\n"\
	"	bne	4f					\n"\
	"	fmoved	fp0,%2@					\n"\
	"	jmp	7f					\n"\
	"4:							\n"\
	"	movel	d1,%2@(4)				\n"\
	"	movel	d0,%2@					\n"\
	"	jmp	7f					\n"\
	"5:							\n"\
	"	cmpb	#1,%0					\n"\
	"	bne	7f					\n"\
	"	cmpb	#0x46,%1				\n"\
	"	bne	6f					\n"\
	"	fmoves	fp0,%2@					\n"\
	"	jmp	7f					\n"\
	"6:							\n"\
	"	movel	d0,%2@					\n"\
	"7:							\n"\
        :							  \
	: "m" ((CALL)->retsize),				  \
	  "m" ((CALL)->rettype),				  \
	  "a" ((CALL)->ret)					  \
	: "d0", "d1", "fp0", "cc", "memory");			  \
	asm volatile ("addw %0,sp" 				  \
	: : "r" ((CALL)->argsize * sizeof(jint)) : "cc")

#endif
