/*
 * i386/univel_svr4/md.h
 * FreeBSD i386 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_univel_svr4_md_h
#define __i386_univel_svr4_md_h

#include "i386/common.h"
#include "i386/threads.h"

/**/
/* Extra exception handling information. */
/**/
#include <siginfo.h>
#include <ucontext.h>
#if defined(HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

/* Function prototype for signal handlers */
#define	SIGNAL_ARGS(sig, sc) int sig, siginfo_t* sip, ucontext_t* sc
#define SIGNAL_CONTEXT_POINTER(scp) ucontext_t* scp
#define GET_SIGNAL_CONTEXT_POINTER(scp) (scp)
#define SIGNAL_PC(scp) ((scp)->uc_mcontext.gregs[EIP])
#define STACK_POINTER(scp) ((scp)->uc_mcontext.gregs[ESP])

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* NCR MP-RAS requires a little initialisation */
extern void init_md(void);
#define INIT_MD()	init_md()

#include "kaffe-unix-stack.h"

#endif
