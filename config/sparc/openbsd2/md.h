/*
 * sparc/openbsd2/md.h
 * OpenBSD Sparc configuration information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_openbsd2_md_h
#define __sparc_openbsd2_md_h

#include "sparc/common.h"
#include "sparc/threads.h"

#undef SP_OFFSET
#define SP_OFFSET 2

/* Function prototype for signal handlers */
#define	SIGNAL_ARGS(sig, sc) int sig, int code, struct sigcontext* sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext* scp
#define GET_SIGNAL_CONTEXT_POINTER(scp) (scp)
#define SIGNAL_PC(scp) ((scp)->sc_pc)

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#endif
