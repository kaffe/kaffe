/*
 * sparc/solaris2/md.h
 * Solaris2 sparc configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_solaris2_md_h
#define __sparc_solaris2_md_h

#include "sparc/common.h"
#include "sparc/threads.h"

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
#define SIGNAL_PC(scp) ((scp)->uc_mcontext.gregs[REG_PC])
#define STACK_POINTER(scp) ((scp)->uc_mcontext.gregs[REG_SP])

#if defined(HAVE_GETRLIMIT)
#define KAFFEMD_STACKSIZE

static inline rlim_t mdGetStackSize(void)
{
  struct rlimit rl;

  if (getrlimit(RLIMIT_STACK, &rl) < 0)
    return 0;
  else
    return rl.rlim_cur;
}
#endif

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#endif
