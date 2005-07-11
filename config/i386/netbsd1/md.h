/*
 * i386/netbsd1/md.h
 * NetBSD i386 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003, 2004, 2005
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_netbsd1_md_h
#define __i386_netbsd1_md_h

#include "i386/common.h"
#include "i386/threads.h"

#if defined(HAVE_SIGNAL_H)
#include <signal.h>
#endif
#if defined(HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif
#if defined(HAVE_SYS_SIGNAL_H)
#include <sys/signal.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif


/*
 * Redefine stack pointer offset.
 */
#undef SP_OFFSET
#define SP_OFFSET 2

#if defined(HAVE_SYS_UCONTEXT_H)
#include <sys/ucontext.h>

/* older netbsd's could have this macro missing, so we provide it */
#ifdef _UC_MACHINE_SP
#define _UC_MACHINE_SP(uc)      ((uc)->uc_mcontext.__gregs[_REG_UESP])
#endif
#ifdef _UC_MACHINE_PC
#define _UC_MACHINE_PC(uc)      ((uc)->uc_mcontext.__gregs[_REG_EIP])
#endif

#define SIGCONTEXT ucontext_t

#define SIGNAL_ARGS(sig, sc) int sig, siginfo_t *__si, void *sc
#define SIGNAL_CONTEXT_POINTER(scp) SIGCONTEXT *scp
#define SIGNAL_PC(scp) _UC_MACHINE_PC(((SIGCONTEXT *)(scp)))
#define STACK_POINTER(scp) _UC_MACHINE_SP(((SIGCONTEXT *)(scp)))
#define GET_SIGNAL_CONTEXT_POINTER(sc) (sc)

#else

#define SIGNAL_ARGS(sig, sc) int sig, int __code, struct sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext *scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (sc)
#define SIGNAL_PC(scp) (scp)->sc_pc
#define STACK_POINTER(scp) (scp)->sc_sp

#endif

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#include "kaffe-unix-stack.h"

#endif

