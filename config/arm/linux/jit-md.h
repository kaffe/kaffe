/*
 * arm/linux/jit-md.h
 * ARM Linux JIT configuration information.
 *
 * Copyright (c) 1996, 1997, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __arm_linux_jit_md_h
#define __arm_linux_jit_md_h

/**/
/* Include common information. */
/**/
#include "arm/jit.h"

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

/*
 * newer Linux kernel actually implement SA_SIGINFO.
 * But we don't need it, so let's turn it off
 */
#if defined(SA_SIGINFO)
#undef SA_SIGINFO
#endif

/* It looks like the linux kernel sets r0 to the signal number
 * and passes a pointer to the context as the fourth argument
 * use this hack to account for that.  -- gback
 *
 * Undef when this gets fixed -- check arch/arm/kernel/signal.c
 */
#define ARM_LINUX_HACK

/* Function prototype for signal handlers */
#if defined(HAVE_STRUCT_SIGCONTEXT_STRUCT) && !defined(__GLIBC__)
/* Linux < 2.1.1 */
#if defined(ARM_LINUX_HACK)
#define EXCEPTIONPROTO                                            \
        int sig, int r1, int r2, int r3, struct sigcontext_struct ctx
#else
#define	EXCEPTIONPROTO							\
	int sig, struct sigcontext_struct ctx
#endif /* ARM_LINUX_HACK */

#elif defined(HAVE_STRUCT_SIGCONTEXT) || defined(__GLIBC__)
/* Linux >= 2.1.1  or Linux 2.0.x with glibc2 */
#if defined(ARM_LINUX_HACK)
#define EXCEPTIONPROTO                                                  \
        int sig, int r1, int r2, int r3, struct sigcontext ctx
#else
#define	EXCEPTIONPROTO							\
	int sig, struct sigcontext ctx
#endif /* ARM_LINUX_HACK */
#else
#error Do not know how to define EXCEPTIONPROTO
#endif

/* Get the first exception frame from a signal handler */
#define	EXCEPTIONFRAME(f, c)						\
	(f).retfp = (c).reg.ARM_fp;					\
	(f).retpc = (c).reg.ARM_pc;

#endif
