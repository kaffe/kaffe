/*
 * i386/solaris2/md.h
 * Solaris2 i386 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_solaris2_md_h
#define __i386_solaris2_md_h

#include "i386/common.h"
#include "i386/threads.h"

#define	SIGNAL_ARGS(sig, sc) int sig, siginfo_t* sip, ucontext_t* sc
#define SIGNAL_CONTEXT_POINTER(scp) struct ucontext_t * scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (sc)
#define SIGNAL_PC(scp) ((scp)->uc_mcontext.gregs[EIP])

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#endif
