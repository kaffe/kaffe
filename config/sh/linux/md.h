/*    
 * sh/linux/md.h
 * Linux SuperH configuration information.
 *
 * Copyright (c) 2001
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *	Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
 
#if ! defined(__sh_linux_md_h)
#define __sh_linux_md_h
 
#include "sh/common.h"
#include "sh/sysdepCallMethod.h"
#include "sh/threads.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#define EXCEPTIONPROTO  	int sig, int d1, int d2, int d3, struct sigcontext ctx

#define	EXCEPTIONFIXRETURN()	asm volatile("mov.l %2,@%0\n\tmov.l %1,@(4,%0)" : : "r" ((int)&ctx - 8), "r" (ctx.sc_regs[14]), "r" (ctx.sc_pc));

#define	EXCEPTIONPC()		(ctx.sc_pc)

/* The Linux implementation of this provides a large amount
 * of information for real-time stuff.  We don't need that and
 * the standard version returns the sigcontext which is all
 * we're interested in.  So just turn this flag off.
 */
#if defined(SA_SIGINFO)
#undef  SA_SIGINFO
#endif

/*
 * No floating point support - so emulate
 */
#define	HAVE_NO_FLOATING_POINT	1

#undef SA_SIGINFO

#define SIGNAL_ARGS(sig, sc) int sig, int d1, int d2, int d3, struct  sigcontext sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext *scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) &sc
#define SIGNAL_PC(scp) scp->sc_pc

#endif /* ! defined(__sh_linux_md_h) */
