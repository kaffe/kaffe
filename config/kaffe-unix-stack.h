/*
 * config/kaffe-unix-stack.h
 * Commmon code for handling stack size on SYSV/BSD compliant OS.
 * 
 * Copyright (c) 2004
 *       Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
#ifndef _kaffe_unix_stack_h
#define _kaffe_unix_stack_h

#include "config.h"

#if defined(HAVE_SETRLIMIT) && defined(HAVE_GETRLIMIT)

#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#if defined(HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#define KAFFEMD_STACK_INFINITE RLIM_INFINITY
#define KAFFEMD_STACK_ERROR 0

#define KAFFEMD_STACKSIZE

static inline rlim_t mdGetStackSize(void)
{
  struct rlimit rl;

  if (getrlimit(RLIMIT_STACK, &rl) < 0)
    return KAFFEMD_STACK_ERROR;
  else
    return rl.rlim_cur;
}


static inline void mdSetStackSize(rlim_t limit)
{
  struct rlimit rl;

  getrlimit(RLIMIT_STACK, &rl);
  rl.rlim_cur = limit;
  setrlimit(RLIMIT_STACK, &rl);
}

#endif

#endif
