/*
 * m68k/linux/md.h
 * Linux m68k configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __m68k_linux_md_h
#define __m68k_linux_md_h

#include "m68k/common.h"
#include "m68k/threads.h"

/*
 * Redefine stack pointer offset.
 */
#undef  SP_OFFSET
#define SP_OFFSET	14

/**/
/* Extra exception handling information. */
/**/
#include <signal.h>

#define SIGNAL_ARGS(sig, sc) int sig, int code, struct sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext *scp
#define GET_SIGNAL_CONTEXT_POINTER(scp) (scp)
#define SIGNAL_PC(scp) ((uintp)(scp)->sc_pc)

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* Linux requires a little initialisation */
extern void init_md(void);
#define	INIT_MD()	init_md()

#endif
