/*
 * i386/linux/md.h
 * Linux i386 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __ki386_linux_md_h
#define __ki386_linux_md_h

#include "i386/common.h"
#include "i386/threads.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* Linux requires a little initialisation */
extern void init_md(void);
#define	INIT_MD()	init_md()

#define SIGNAL_ARGS(sig, sc) int sig, struct sigcontext sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext * scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (&sc)
#define SIGNAL_PC(scp) scp->eip

#endif
