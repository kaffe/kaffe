/*
 * ia64/freebsd2/md.h
 * FreeBSD ia64 configuration information.
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

#ifndef __ia64_freebsd_md_h
#define __ia64_freebsd_md_h

#include "ia64/common.h"
#include "ia64/threads.h"

#undef	SP_OFFSET
#define	SP_OFFSET	11
#undef  FP_OFFSET
#define FP_OFFSET	12

#define SIGNAL_ARGS(sig, sc)		int sig, int __code, struct sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp)	struct sigcontext *scp
#define GET_SIGNAL_CONTEXT_POINTER(sc)  (sc)
#define SIGNAL_PC(scp)			(scp)->sc_pc
#define STACK_POINTER(scp)		(scp)->sc_special.sp

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#include "kaffe-unix-stack.h"

#if defined(HAVE_SYS_SYSCTL_H)
#define KAFFEMD_STACKEND
extern void *mdGetStackEnd(void);
#endif

#endif
