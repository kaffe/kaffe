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
#include "arm/threads.h"


/* It looks like the linux kernel sets r0 to the signal number
 * and passes a pointer to the context as the fourth argument
 * use this hack to account for that.  -- gback
 *
 * Undef when this gets fixed -- check arch/arm/kernel/signal.c
 */
#define ARM_LINUX_HACK

/* Function prototype for signal handlers */
#if defined(HAVE_STRUCT_SIGCONTEXT_STRUCT) && !defined(__GLIBC__)
/* Linux < 2.1.1 */
#if defined(ARM_LINUX_HACK)
#define SIGNAL_ARGS(sig, ctx) \
        int sig, int r1, int r2, int r3, struct sigcontext_struct ctx
#else
#define	SIGNAL_ARGS(sig, ctx) \
	int sig, struct sigcontext_struct ctx
#endif /* ARM_LINUX_HACK */

#elif defined(HAVE_STRUCT_SIGCONTEXT) || defined(__GLIBC__)
/* Linux >= 2.1.1  or Linux 2.0.x with glibc2 */
#if defined(ARM_LINUX_HACK)
#define SIGNAL_ARGS(sig, ctx) \
        int sig, int r1, int r2, int r3, struct sigcontext ctx
#else
#define	SIGNAL_ARGS(sig, ctx) \
	int sig, struct sigcontext ctx
#endif /* ARM_LINUX_HACK */
#else
#error Do not know how to define EXCEPTIONPROTO
#endif

#define GET_SIGNAL_CONTEXT_POINTER(ctx) (&ctx)

#if defined(HAVE_REG_SIGCONTEXT)
#define SIGNAL_PC(ctx) (ctx)->reg.ARM_pc
#else
#define SIGNAL_PC(ctx) (ctx)->arm_pc
#endif

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

/* arm/linux/elf (NetWinder) */
#define SP_OFFSET               20
#define FP_OFFSET               19

#endif
