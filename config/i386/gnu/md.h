/*
 * i386/gnu/md.h
 * Linux i386 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003, 2004
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __ki386_gnu_md_h
#define __ki386_gnu_md_h

#include "i386/common.h"
#include "i386/sysdepCallMethod.h"
#include "i386/threads.h"

/* Function prototype for signal handlers */
#if defined(__GLIBC__)
#define	SIGNAL_ARGS(sig, sc) int sig, int code, struct sigcontext* sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext * scp
#define GET_SIGNAL_CONTEXT_POINTER(scp) (scp)
#else
#error Do not know how to define EXCEPTIONPROTO
#endif

#define SIGNAL_PC(scp) (scp)->eip
#define STACK_POINTER(scp) (scp)->esp

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* Linux requires a little initialisation */
extern void init_md(void);
#define	INIT_MD()	init_md()

#endif
