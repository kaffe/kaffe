/*    
 * arm/linux/md.h
 * Linux arm configuration information.
 *
 * Copyright (c) 1996, 1997
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
 
#ifndef __arm_linux_md_h
#define __arm_linux_md_h
 
#include "arm/common.h"
#include "arm/sysdepCallMethod.h"
#include "arm/threads.h"

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
	int sig, int arm_r1, int arm_r2, int arm_r3, struct sigcontext_struct ctx
#elif defined(HAVE_STRUCT_SIGCONTEXT) || defined(__GLIBC__)
/* Linux >= 2.1.1  or Linux 2.0.x with glibc2 */
#define	SIGNAL_ARGS(sig, ctx) \
	int sig, int arm_r1, int arm_r2, int arm_r3, struct sigcontext ctx
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
 * figured by looking at sysdeps/arm/bits/setjmp.h
 * and sysdeps/arm/setjmp.S from glibc
 */ 
#if defined(__XSCALE__) || defined(HAVE_XSCALE)
#define SP_OFFSET	20
#else
#define SP_OFFSET	8
#endif /* defined(__XSCALE__) || defined(HAVE_XSCALE) */

#define FP_OFFSET	7

#endif
