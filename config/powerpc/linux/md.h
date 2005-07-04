/*
 * powerpc/linux/md.h
 * Linux PowerPC configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *      The kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __powerpc_linux_md_h
#define __powerpc_linux_md_h

#include "powerpc/common.h"
#include "powerpc/threads.h"

#if defined(HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif
#if defined(HAVE_SIGNAL_H)
#include <signal.h>
#endif
#if defined(HAVE_ASM_SIGCONTEXT_H) && !defined(__GLIBC__)
#include <asm/sigcontext.h>
#endif

/*
 * We don't need a SA_SIGINFO flag to get a proper
 * sigcontext pointer in a signal handlers.
 */
#if defined(SA_SIGINFO)
#undef SA_SIGINFO
#endif

/* Linux requires a little initialisation */
extern void init_md(void);
#define	INIT_MD()	init_md()

#ifdef SP_OFFSET
#undef SP_OFFSET
#endif
#define SP_OFFSET	0

#include "sigcontextinfo.h"

#define SIGNAL_ARGS(sig, sc) int sig, struct sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext *scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (sc)
#define SIGNAL_PC(scp) (GET_PC(scp))
#define STACK_POINTER(scp) (GET_STACK(scp))
#undef HAVE_SIGALTSTACK

#include "kaffe-unix-stack.h"

/* align data types to their size */
#define   ALIGNMENT_OF_SIZE(S)    (S)

#if defined(__GLIBC__)
#define KAFFEMD_STACKEND
extern void *mdGetStackEnd(void);
#endif /* __GLIBC__ */


#endif
