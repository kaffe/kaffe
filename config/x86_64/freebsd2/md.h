/*
 * x86_64/freebsd2/md.h
 * FreeBSD AMD64 configuration information.
 *
 * Copyright (c) 2002
 *	MandrakeSoft.  All rights reserved.
 *
 * Copyright (c) 2001
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __amd64_freebsd_md_h
#define __amd64_freebsd_md_h

#include "x86_64/common.h"
#include "x86_64/threads.h"

#undef	SP_OFFSET
#define	SP_OFFSET	2
#undef  FP_OFFSET
#define FP_OFFSET	3

#define SIGNAL_ARGS(sig, sc)		int sig, int __code, struct sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp)	struct sigcontext *scp
#define GET_SIGNAL_CONTEXT_POINTER(sc)  (sc)
#define SIGNAL_PC(scp)			(scp)->sc_pc
#define STACK_POINTER(scp)		(scp)->sc_rsp

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#include "kaffe-unix-stack.h"

#if defined(HAVE_SYS_SYSCTL_H)
#define KAFFEMD_STACKEND
extern void *mdGetStackEnd(void);
#endif

#endif
