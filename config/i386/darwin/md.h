/*
 * i386/darwin/md.h
 * Darwin x86 configuration information.
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

#ifndef __i386_darwin_md_h
#define __i386_darwin_md_h

#include "i386/common.h"
#include "i386/threads.h"

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
#define SP_OFFSET	9

#define SIGNAL_ARGS(sig, sc) int sig, int code, struct sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext *scp
#define GET_SIGNAL_CONTEXT_POINTER(scp) (scp)
#define SIGNAL_PC(scp) ((scp)->sc_eip)
#define STACK_POINTER(scp) ((scp)->sc_esp)

#undef HAVE_SIGALTSTACK
#define STACK_STRUCT struct sigaltstack

#define DARWIN

#include "kaffe-unix-stack.h"

#endif
