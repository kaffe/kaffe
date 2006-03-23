/*
 * i386/openbsd2/md.h
 * OpenBSD i386 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003, 2004
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_openbsd2_md_h
#define __i386_openbsd2_md_h

#include "i386/common.h"
#include "i386/threads.h"

#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#if defined(HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

/*
 * Redefine stack pointer offset.
 */
#undef SP_OFFSET
#define SP_OFFSET 2

/* Define signal context macros for xprofiling */
#define SIGNAL_ARGS(sig, sc) int sig, int __code, struct sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext *scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (sc)
#define SIGNAL_PC(scp) (scp)->sc_pc
#define STACK_POINTER(scp) (scp)->sc_sp

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#define KAFFEMD_STACK_ERROR 0
#define KAFFEMD_STACK_INFINITE KAFFEMD_STACK_ERROR

#define KAFFEMD_STACKSIZE
extern size_t mdGetStackSize(void);

/* this is only used for the main thread and is ok for that */
/* this may change with rthreads when thats done */
static inline void mdSetStackSize(rlim_t limit)
{
  struct rlimit rl;

  getrlimit(RLIMIT_STACK, &rl);
  rl.rlim_cur = limit;
  setrlimit(RLIMIT_STACK, &rl);
}

#define KAFFEMD_STACKEND
extern void *mdGetStackEnd(void);

#endif
