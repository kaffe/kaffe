/*
 * alpha/linux/jit-md.h
 * Linux Alpha JIT configuration information.
 *
 * Copyright (c) 1996, 1997
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

/* Function prototype for signal handlers */
#if defined(HAVE_STRUCT_SIGCONTEXT_STRUCT)
/* Linux < 2.1.1 */
#define	EXCEPTIONPROTO							\
	int sig, int code, struct sigcontext_struct *ctx

#elif defined(HAVE_STRUCT_SIGCONTEXT)
/* Linux >= 2.1.1 */
#define	EXCEPTIONPROTO							\
	int sig, int code, struct sigcontext *ctx
#endif

/* Get the first exception frame from a signal handler */
#define	EXCEPTIONFRAME(f, c)						\
	do {								\
		exceptionFrame *_cur = TCTX(currentThread)->exceptPtr;	\
		if (_cur)						\
			(f) = *_cur;					\
		else {							\
			(f).retfp = (c)->sc_regs[15];			\
			(f).retpc = (c)->sc_pc;				\
		}							\
	} while (0)

#endif
