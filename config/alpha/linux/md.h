/*
 * alpha/linux/md.h
 * Linux Alpha configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __alpha_linux_md_h
#define __alpha_linux_md_h

#include "alpha/common.h"
#include "alpha/threads.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#undef SP_OFFSET
#define SP_OFFSET 8

#include "sigcontextinfo.h"

#define SIGNAL_ARGS(sig, sc) int sig, int alpha_dummy, struct sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext * scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (sc)
#define SIGNAL_PC(scp) (GET_PC((*scp)))
#define STACK_POINTER(scp) (GET_STACK((*scp)))

/* Linux requires a little initialisation */
extern void init_md(void);
#define	INIT_MD()	init_md()

#endif
