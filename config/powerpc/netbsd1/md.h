/*
 * powerpc/netbsd1/md.h
 * NetBSD PowerPC configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2005
 *      The kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __powerpc_netbsd1_md_h
#define __powerpc_netbsd1_md_h

#include "powerpc/common.h"
#include "powerpc/threads.h"

#include <signal.h>

#include <sys/ucontext.h>
#include <machine/mcontext.h>

/* older netbsd's could have this macro missing, so we provide it */
#ifdef _UC_MACHINE_SP
#define _UC_MACHINE_SP(uc)      ((uc)->uc_mcontext.__gregs[_REG_R1])
#endif
#ifdef _UC_MACHINE_PC
#define _UC_MACHINE_PC(uc)      ((uc)->uc_mcontext.__gregs[_REG_PC])
#endif

#if (__GNUC__ == 2 && __GNUC_MINOR__ == 91)
/* egcs-1.1.2 aka gcc-2.91 can't compile locks.c with "cc" constraint */
extern int __egcs_cmpxchg(void **A, void *O, void *N);

#undef COMPARE_AND_EXCHANGE
#define COMPARE_AND_EXCHANGE(A,O,N)		\
	__egcs_cmpxchg((void**)(A), (void*)(O), (void*)(N))
#endif /* ecgs 1.1.2 - gcc 2.91 */


#if defined(JTHREAD_JBLEN) && (JTHREAD_JBLEN > 0)

#undef SP_OFFSET
#define SP_OFFSET 0

extern void kaffe_longjmp (void *jb, int);
extern int kaffe_setjmp (void *jb);

#define JTHREAD_SETJMP(jb)		kaffe_setjmp(jb)
#define JTHREAD_LONGJMP(jb, val)	kaffe_longjmp(jb, val)

#else /* JTHREAD_JBLEN && JTHREAD_JBLEN > 0 */

#undef SP_OFFSET
#define SP_OFFSET	3

/* JTHREAD_JMPBUFF can't be redefined here as it may be already use in
   structure vmException */

#endif /* jthread_jblen */

#define SIGCONTEXT ucontext_t

#define SIGNAL_ARGS(sig, sc) int sig, siginfo_t *__si, void *sc
#define SIGNAL_CONTEXT_POINTER(scp) SIGCONTEXT *scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (sc)
#define SIGNAL_PC(scp) _UC_MACHINE_PC(((SIGCONTEXT *)(scp)))
#define STACK_POINTER(scp) _UC_MACHINE_SP(((SIGCONTEXT *)(scp)))

#undef HAVE_SIGALTSTACK



#include "kaffe-unix-stack.h"

/* align data types to their size */
#define   ALIGNMENT_OF_SIZE(S)    (S)



#endif /* include guard */
