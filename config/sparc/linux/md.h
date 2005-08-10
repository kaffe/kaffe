/*
 * sparc/linux/md.h
 * Linux sparc configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_linux_md_h
#define __sparc_linux_md_h

#include "sparc/common.h"
#include "sparc/threads.h"

#undef	SP_OFFSET
#undef	FP_OFFSET
#define	SP_OFFSET		0
#define	FP_OFFSET		1

struct _jthread;
void sparcLinuxContextSwitch(struct _jthread* from, struct _jthread* to);

#define	CONTEXT_SWITCH(F,T)	sparcLinuxContextSwitch(F, T)

/**/
/* Extra exception handling information. */
/**/
#if defined(HAVE_FEATURES_H)
#include <features.h>
#endif 
#if defined(HAVE_SIGCONTEXT_H)
#include <sigcontext.h> 
#endif
#include <asm/ptrace.h>
#if defined(HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined(__arch64__)
#include "sigcontextinfo64.h"
#else /* !defined(__arch64__) */
#include "sigcontextinfo32.h"
#endif

/* Function prototype for signal handlers */
#undef SA_SIGINFO
#define	SIGNAL_ARGS(sig, sc) int sig, struct sigcontext* sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext* scp
#define GET_SIGNAL_CONTEXT_POINTER(scp) (scp)
#define SIGNAL_PC(scp) (GET_PC(scp))
#define STACK_POINTER(scp) (GET_STACK(scp))

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* Linux requires a little initialisation */
extern void init_md(void);
#define	INIT_MD()	init_md()

#include "kaffe-unix-stack.h"

#endif
