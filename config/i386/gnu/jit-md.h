/*
 * i386/gnu/jit-md.h
 * Linux i386 JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_gnu_jit_md_h
#define __i386_gnu_jit_md_h

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
#if defined(HAVE_SIGNAL_H)
#include <signal.h>
#endif
#if defined(HAVE_SIGCONTEXT_H)
#include <sigcontext.h>
#endif

/*
 * newer Linux kernel actually implement SA_SIGINFO.
 * But we don't need it, so let's turn it off
 *
 * [Don't know if it has any realtion to the HURD, but it
 * doesn't hurt, su just leave it.]
 */
#if defined(SA_SIGINFO)
#undef SA_SIGINFO
#endif

#define	EXCEPTIONPROTO SIGNAL_ARGS(sig, ctx)

/* Get the first exception frame from a signal handler */
#define	EXCEPTIONFRAME(f, c)						\
	(f).retbp = (c)->sc_ebp;					\
	(f).retpc = (c)->sc_eip + 1

#endif
