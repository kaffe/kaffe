/*
 * powerpc/openbsd2/md.h
 * OpenBSD PowerPC configuration information.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __powerpc_openbsd2_md_h
#define __powerpc_openbsd2_md_h

#include "powerpc/common.h"
#include "powerpc/threads.h"

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#if defined(HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif
#if defined(HAVE_SYS_SIGNAL_H)
#include <sys/signal.h>
#endif

#undef SP_OFFSET
#define SP_OFFSET      1

#define SIGNAL_ARGS(sig, sc) int sig, siginfo_t *__si, struct sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext *scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (sc)
#define STACK_POINTER(scp) ((scp)->sc_frame.fixreg[1])

#undef HAVE_SIGALTSTACK

/* align data types to their size */
#define   ALIGNMENT_OF_SIZE(S)    (S)

#if defined(KAFFE_SYSTEM_UNIX_PTHREADS)

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

#else /* KAFFE_SYSTEM_UNIX_PTHREADS */

#include "kaffe-unix-stack.h"

#endif /* KAFFE_SYSTEM_UNIX_PTHREADS */

#endif
