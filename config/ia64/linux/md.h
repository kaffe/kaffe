/*
 * ia64/linux/md.h
 * Linux IA-64 configuration information.
 *
 * Copyright (c) 2001
 *	MandrakeSoft.  All rights reserved.
 *
 * Copyright (c) 2001
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __ia64_linux_md_h
#define __ia64_linux_md_h

#include <setjmp.h>
#include "ia64/common.h"
#include "ia64/threads.h"

/* Linux requires a little initialisation */
extern void init_md(void);
#define	INIT_MD() init_md()

extern int IA64_context_save(jmp_buf env);
extern void IA64_context_restore(jmp_buf env, int val);

#if 0
#undef  JTHREAD_CONTEXT_SAVE
#define JTHREAD_CONTEXT_SAVE(env)		IA64_context_save((env))
#undef  JTHREAD_CONTEXT_RESTORE
#define JTHREAD_CONTEXT_RESTORE(env, val)	IA64_context_restore((env), (val))
#endif

#define SIGNAL_ARGS(sig, scp)			int sig, siginfo_t *sip, struct sigcontext *scp
#define SIGNAL_CONTEXT_POINTER(scp)		struct sigcontext *scp
#define GET_SIGNAL_CONTEXT_POINTER(sc)		(sc)
#define SIGNAL_PC(scp)				((scp)->sc_ip & ~0x3ULL)

#endif
