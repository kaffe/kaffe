/*
 * powerpc/linux/md.h
 * Linux PowerPC configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __powerpc_linux_md_h
#define __powerpc_linux_md_h

#include "powerpc/common.h"
#include "powerpc/threads.h"


/* Linux requires a little initialisation */
extern void init_md(void);
#define	INIT_MD()	init_md()

#ifdef SP_OFFSET
#undef SP_OFFSET
#endif
#define SP_OFFSET	0

#define SIGNAL_ARGS(sig, sc) int sig
#undef HAVE_SIGALTSTACK

#if defined(HAVE_GETRLIMIT)
#define KAFFEMD_STACKSIZE

static inline rlim_t mdGetStackSize(void)
{
  struct rlimit rl;

  // The soft limit is always the lower limit.
  // Use it by default.
  if (getrlimit(RLIMIT_STACK, &rl) < 0)
    return 0;
  else
    return rl.rlim_cur;
}
#endif


#endif
