/*
 * sparc/openbsd2/md.h
 * OpenBSD Sparc configuration information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_openbsd2_md_h
#define __sparc_openbsd2_md_h

#include "sparc/common.h"
#include "sparc/threads.h"
#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#if defined(HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#undef SP_OFFSET
#define SP_OFFSET 1

/* Function prototype for signal handlers */
#undef SA_SIGINFO
#define	SIGNAL_ARGS(sig, sc) int sig, siginfo_t *__sip, struct sigcontext* sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext* scp
#define GET_SIGNAL_CONTEXT_POINTER(scp) (scp)
#define SIGNAL_PC(scp) ((scp)->sc_pc)
#define STACK_POINTER(scp) ((scp)->sc_sp)

#define STACK_STRUCT struct sigaltstack

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#if defined(KAFFE_SYSTEM_UNIX_PTHREADS)

#define KAFFEMD_STACK_ERROR 0
#define KAFFEMD_STACK_INFINITE KAFFEMD_STACK_ERROR

#define KAFFEMD_STACKSIZE
extern size_t mdGetStackSize(void);

#define KAFFEMD_DISABLE_SETSTACKSIZE

#define KAFFEMD_STACKEND
extern void *mdGetStackEnd(void);

#else /* KAFFE_SYSTEM_UNIX_PTHREADS */

#include "kaffe-unix-stack.h"

#endif /* KAFFE_SYSTEM_UNIX_PTHREADS */

#endif
