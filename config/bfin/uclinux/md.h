/*    
 * bfin/linux/md.h
 * Linux bfin configuration information.
 *
 * Copyright (c) 1996, 1997
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
 
#ifndef __bfin_linux_md_h
#define __bfin_linux_md_h
 
#include "bfin/common.h"
#include "bfin/sysdepCallMethod.h"
#include "bfin/threads.h"

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef HAVE_SIGCONTEXT_H
#include <sigcontext.h>
#endif

/*
 * newer Linux kernel actually implement SA_SIGINFO.
 * But we don't need it, so let's turn it off
 */
#if defined(SA_SIGINFO)
#undef SA_SIGINFO
#endif

/* Function prototype for signal handlers */
#if defined(HAVE_STRUCT_SIGCONTEXT_STRUCT) && !defined(__GLIBC__)
/* Linux < 2.1.1 */
#define	SIGNAL_ARGS(sig, ctx) \
	int sig, int bfin_r1, int bfin_r2, int bfin_r3, struct sigcontext_struct ctx
#elif defined(HAVE_STRUCT_SIGCONTEXT) || defined(__GLIBC__)
/* Linux >= 2.1.1  or Linux 2.0.x with glibc2 */
#define	SIGNAL_ARGS(sig, ctx) \
	int sig, int bfin_r1, int bfin_r2, int bfin_r3, struct sigcontext ctx
#else
#error Do not know how to define SIGNAL_ARGS
#endif

#include "sigcontextinfo.h"

#define GET_SIGNAL_CONTEXT_POINTER(ctx) (&ctx)

#define SIGNAL_PC(ctx) (GET_PC((*ctx)))
#define STACK_POINTER(ctx) (GET_STACK((*ctx)))

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* Linux requires a little initialisation */
extern void init_md(void);
#define INIT_MD()       init_md()

#define unix

/* Define CREATE_NULLPOINTER_CHECKS in md.h when your machine cannot use the
 * MMU for detecting null pointer accesses
 *
 * Actually, when debugging, it would be nice to don't catch unexpected
 * exception, so...
 */

#ifdef  KAFFE_VMDEBUG
#define CREATE_NULLPOINTER_CHECKS
#endif

#undef SP_OFFSET
#undef FP_OFFSET

/*
 * figured by looking at sysdeps/bfin/bits/setjmp.h
 * and sysdeps/bfin/setjmp.S from glibc
 */ 
#define SP_OFFSET	8

#define FP_OFFSET	7

#endif
