/*
 * i386/linux/jit-md.h
 * Linux i386 JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_linux_jit_md_h
#define __i386_linux_jit_md_h

/**/
/* Include common information. */
/**/
#include "i386/jit.h"

/**/
/* Extra exception handling information. */
/**/
#if defined(HAVE_FEATURES_H)
#include <features.h>
#endif
#if defined(HAVE_ASM_SIGNAL_H) && !defined(__GLIBC__)
#include <asm/signal.h>
#endif
#if defined(HAVE_SIGNAL_H)
#include <signal.h>
#endif
#if defined(HAVE_ASM_SIGCONTEXT_H) && !defined(__GLIBC__)
#include <asm/sigcontext.h>
#endif
#if defined(HAVE_SIGCONTEXT_H)
#include <sigcontext.h>
#endif

#if !defined (SA_SIGINFO)
/* Function prototype for signal handlers */
#if defined(HAVE_STRUCT_SIGCONTEXT_STRUCT) && !defined(__GLIBC__)
/* Linux < 2.1.1 */
#define	EXCEPTIONPROTO							\
	int sig, struct sigcontext_struct ctx

#elif defined(HAVE_STRUCT_SIGCONTEXT) || defined(__GLIBC__)
/* Linux >= 2.1.1  or Linux 2.0.x with glibc2 */
#define	EXCEPTIONPROTO							\
	int sig, struct sigcontext ctx
#endif

/* Get the first exception frame from a signal handler */
#define	EXCEPTIONFRAME(f, c)						\
	(f).retbp = (c).ebp;						\
	(f).retpc = (c).eip + 1

#else

#include <asm/ucontext.h>

/* Function prototype for signal handlers */
#define	EXCEPTIONPROTO							\
	int sig, struct siginfo *sip, struct ucontext *ctx

/* Get the first exception frame from a signal handler */
#define	EXCEPTIONFRAME(f, c)						\
	(f).retbp = (c)->uc_mcontext.ebp;				\
	(f).retpc = (c)->uc_mcontext.eip + 1
#endif

#endif
