/*
 * i386/sco3.2v5/md.h
 * SCO i386 configuration information.
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

#ifndef __i386_sco3_2v5_md_h
#define __i386_sco3_2v5_md_h

#include "i386/common.h"
#include "i386/threads.h"

#if defined(HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#define	POSIX_SIGNALS

/**/
/* Extra exception handling information. */
/**/
#include <siginfo.h>
#include <sys/regset.h>		/* Bug in ucontext.h requires this */
#include <signal.h>		/* Bug in ucontext.h requires this */
#include <ucontext.h>

#define SIGNAL_ARGS(sig, sc) int sig, siginfo_t* sip, ucontext_t* ctx
#define SIGNAL_CONTEXT_POINTER(scp) ucontext_t* ctx
#define GET_SIGNAL_CONTEXT_POINTER(scp) (scp)
#define SIGNAL_PC(scp) ((scp)->uc_mcontext.regs[EIP])
#define STACK_POINTER(scp) ((scp)->uc_mcontext.regs[ESP])

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#include "kaffe-unix-stack.h"

#endif
