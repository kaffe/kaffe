/*
 * x86_64/linux/md.h
 * Linux X86-64 configuration information.
 *
 * Copyright (c) 2002
 *	MandrakeSoft.  All rights reserved.
 *
 * Copyright (c) 2001
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __x86_64_linux_md_h
#define __x86_64_linux_md_h

#include "x86_64/common.h"
#include "x86_64/threads.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* Linux requires a little initialisation */
extern void init_md(void);
#define	INIT_MD() init_md()

#include "sigcontextinfo.h"

#define SIGNAL_ARGS(sig, scp)           int sig, siginfo_t *sip, ucontext_t *scp
#define SIGNAL_CONTEXT_POINTER(scp)     ucontext_t *scp
#define GET_SIGNAL_CONTEXT_POINTER(sc)  (sc)
#define SIGNAL_PC(scp)                  (GET_PC((*scp)))
#define STACK_POINTER(scp)		(GET_STACK((*scp)))

#endif
