/*
 * alpha/linux/jit-md.h
 * Linux Alpha JIT configuration information.
 *
 * Copyright (c) 2001
 *	Edouard G. Parmelan.  All rights reserved.
 *
 * Copyright (c) 1996, 1997, 2001
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __alpha_linux_jit_md_h
#define __alpha_linux_jit_md_h

/**/
/* Include common information. */
/**/
#include "alpha/jit.h"

/**/
/* Extra exception handling information. */
/**/
#if defined(HAVE_FEATURES_H)
#include <features.h>
#endif

#if defined(HAVE_SIGNAL_H)
#include <signal.h>
#endif
#if defined(HAVE_SIGCONTEXT_H)
#include <sigcontext.h>
#endif

#if defined(HAVE_ASM_SIGNAL_H) && !defined(__GLIBC__)
#include <asm/signal.h>
#endif
#if defined(HAVE_ASM_SIGCONTEXT_H) && !defined(__GLIBC__)
#include <asm/sigcontext.h>
#endif

/*
 * newer Linux kernel actually implement SA_SIGINFO.
 * But we don't need it, so let's turn it off
 */
#if defined(SA_SIGINFO)
#undef SA_SIGINFO
#endif

/* Function prototype for signal handlers */
#if defined(HAVE_STRUCT_SIGCONTEXT_STRUCT) && !defined(__GLIBC__)
/* Linux < 2.1.1 */
#define	EXCEPTIONPROTO							\
	int sig, int code, struct sigcontext_struct *ctx

#elif defined(HAVE_STRUCT_SIGCONTEXT) || defined(__GLIBC__)
/* Linux >= 2.1.1  or Linux 2.0.x with glibc2 */
#define	EXCEPTIONPROTO							\
	int sig, int code, struct sigcontext *ctx
#endif


/* Structure of exception frame on stack */
typedef struct _exceptionFrame {
	uintp pc;
	uintp sp;
	uintp fp;
} exceptionFrame;

int __alpha_ra (uintp pc);
exceptionFrame *__alpha_nextFrame (exceptionFrame *frame);

/* Extract PC, FP and SP from the given frame */
#define PCFRAME(f)	((f)->pc)
#define SPFRAME(f)	((f)->sp)
#define FPFRAME(f)	((f)->fp)

/* Get the first exception frame from a subroutine call */
#define FIRSTFRAME(f, o)			\
	(f).sp = 0;				\
	__alpha_nextFrame(&f)

/* Get the next frame in the chain */
#define NEXTFRAME(f)				\
	__alpha_nextFrame(f)

/* Get the first exception frame from a signal handler */
#define EXCEPTIONFRAME(f, c)				\
	(f).pc = (__alpha_ra ((c)->sc_pc) == -1)	\
	    ? (c)->sc_pc				\
	    : (c)->sc_regs[__alpha_ra ((c)->sc_pc)];	\
	(f).sp = (c)->sc_regs[30];			\
	(f).fp = (c)->sc_regs[15]

#endif
