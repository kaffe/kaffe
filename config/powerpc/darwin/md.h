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
#if defined(HAVE_SYS_SIGNAL_H)
#include <sys/signal.h>
#endif

#undef SP_OFFSET
#define SP_OFFSET	0

#define SIGNAL_ARGS(sig, sc) int sig, int code, struct sigcontext* sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext* scp
#define GET_SIGNAL_CONTEXT_POINTER(scp) (scp)
#define SIGNAL_PC(scp) (scp)->sc_pc
#define STACK_POINTER(scp) (scp)->sc_sp

#define DARWIN

#undef HAVE_SIGALTSTACK
#define STACK_STRUCT struct sigaltstack

#include "kaffe-unix-stack.h"

/* define alignment of 8 byte data types to 4byte boundaries */
#define   ALIGNMENT_OF_SIZE(S)    ((S) < 4 ? (S) : 4)


#endif
