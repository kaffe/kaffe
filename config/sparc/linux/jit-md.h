/*
 * sparc/linux/jit-md.h
 * Linux SPARC JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_linux_jit_md_h
#define __sparc_linux_jit_md_h

/**/
/* Include common information. */
/**/
#include "sparc/jit.h"

/**/
/* Extra exception handling information. */
/**/
#if defined(HAVE_FEATURES_H)
#include <features.h>
#endif 
#if defined(HAVE_ASM_SIGCONTEXT_H) && !defined(__GLIBC)
#include <asm/sigcontext.h>
#endif
#if defined(HAVE_SIGCONTEXT_H)
#include <sigcontext.h> 
#endif

#include <asm/sigcontext.h>

/* Function prototype for signal handlers */
#define	EXCEPTIONPROTO							\
	int sig, __siginfo_t* ctx

/* Get the first exception frame from a signal handler */
#define	EXCEPTIONFRAME(f, c)						\
	(f).retbp = (c)->si_regs.u_regs[UREG_FP];			\
	(f).retpc = (c)->si_regs.pc

#endif
