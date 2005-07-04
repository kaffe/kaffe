/*
 * sparc/netbsd1/md.h
 * NetBSD1 sparc configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_netbsd1_md_h
#define __sparc_netbsd1_md_h

#include <signal.h>

#include "sparc/common.h"
#include "sparc/threads.h"

#include <sys/ucontext.h>

/* older netbsd's could have this macro missing, so we provide it */
#ifdef _UC_MACHINE_SP
/* this is for 32bit it doesn't take in account 64bit */
#define _UC_MACHINE_SP(uc)      ((uc)->uc_mcontext.__gregs[_REG_O6])
#endif
#ifdef _UC_MACHINE_PC
#define _UC_MACHINE_PC(uc)      ((uc)->uc_mcontext.__gregs[_REG_PC])
#endif

#define SIGCONTEXT ucontext_t

#define SIGNAL_ARGS(sig, sc) int sig, siginfo_t *__si, void *sc
#define SIGNAL_CONTEXT_POINTER(scp) SIGCONTEXT *scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (sc)
#define SIGNAL_PC(scp) _UC_MACHINE_PC(((SIGCONTEXT *)(scp)))
#define STACK_POINTER(scp) _UC_MACHINE_SP(((SIGCONTEXT *)(scp)))

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#undef SP_OFFSET
#define SP_OFFSET 2

#endif
