/*
 * i386/gnu/md.h
 * Linux i386 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003-2005
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __ki386_gnu_md_h
#define __ki386_gnu_md_h

#include "i386/common.h"
#include "i386/threads.h"

/**/
/* Extra exception handling information. */
/**/
#if defined(HAVE_FEATURES_H)
#include <features.h>
#endif
#if defined(HAVE_SIGNAL_H)
#include <signal.h>
#endif
#if defined(HAVE_SIGCONTEXT_H)
#include <sigcontext.h>
#endif
#if defined(HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

/* newer Linux kernel actually implement SA_SIGINFO.
 * But we don't need it, so let's turn it off
 *
 * [Don't know if it has any realtion to the HURD, but it
 * doesn't hurt, su just leave it.]
 */
#if defined(SA_SIGINFO)
#undef SA_SIGINFO
#endif

/* Function prototype for signal handlers */
#if defined(__GLIBC__)
#define	SIGNAL_ARGS(sig, sc) int sig, int code, struct sigcontext* sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext * scp
#define GET_SIGNAL_CONTEXT_POINTER(scp) (scp)
#else
#error Do not know how to define EXCEPTIONPROTO
#endif

#define SIGNAL_PC(scp) (scp)->sc_eip
#define STACK_POINTER(scp) (scp)->sc_uesp

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* Linux requires a little initialisation */
extern void init_md(void);
#define	INIT_MD()	init_md()

#include "kaffe-unix-stack.h"

#endif
