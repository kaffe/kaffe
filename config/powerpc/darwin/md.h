/*
 * powerpc/darwin/md.h
 * Darwin PowerPC configuration information.
 *
 * Copyright (c) 2001
 *	Edouard G. Parmelan.  All rights reserved.
 *
 * Copyright (c) 2001
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __powerpc_darwin_md_h
#define __powerpc_darwin_md_h

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


#undef SP_OFFSET
#define SP_OFFSET	0

#if defined(SA_SIGINFO)
#define SIGNAL_ARGS(sig, sc) int sig, siginfo_t *sc
#define SIGNAL_CONTEXT_POINTER(scp) siginfo_t *scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (sc)
#define SIGNAL_PC(scp) 0
#define STACK_POINTER(scp) ((scp)->si_addr)
#else
#warning Some exceptions may not work properly.
#define SIGNAL_ARGS(sig, sc) int sig
#undef HAVE_SIGALTSTACK
#endif

#include "kaffe-unix-stack.h"

#endif
