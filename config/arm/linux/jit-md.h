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

#undef FIRSTFRAME
#define FIRSTFRAME(f, o) (f) = *(exceptionFrame *)__builtin_frame_address(0)

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

#define EXCEPTIONPROTO SIGNAL_ARGS(sig, ctx)

/* Get the first exception frame from a signal handler */
#if defined(HAVE_REG_SIGCONTEXT)
#define	EXCEPTIONFRAME(f, c)						\
	(f).retfp = (c).reg.ARM_fp;					\
	(f).retpc = (c).reg.ARM_pc;

#else
/* This one is for Debian */
#define	EXCEPTIONFRAME(f, c)						\
	(f).retfp = (c).arm_fp;						\
	(f).retpc = (c).arm_pc;
#endif

#endif
