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

/*
 * sysdepCallMethod supports:
 *
 *	Calling sequences for linux and netbsd1 are same, except for
 *	the place of return values. The float/double values are in
 *	fp0 (linux) or d0/d1 (netbsd1).
 *
 *	Still I do not understand 'asm' statement well, and the
 *	statement below is a 'because it works' version.
 */
//	Linux version
static inline void sysdepCallMethod(callMethodInfo *call)
{
	int extraargs[(call)->nrargs];
	register int d0 asm ("d0");
	register int d1 asm ("d1");
	register double f0d asm ("fp0");
	register float f0f asm ("fp0");
	int *res;
	int *args = extraargs;
	int argidx;
	for(argidx = 0; argidx < (call)->nrargs; ++argidx) {
		if ((call)->callsize[argidx])
			*args++ = (call)->args[argidx].i;
		else
			*args++ = (call)->args[argidx-1].j;
	}
	asm volatile ("jsr	%2@\n"
		      : "=r" (d0), "=r" (d1)
		      : "a" ((call)->function)
		      : "cc", "memory");
	if ((call)->retsize != 0) {
		res = (int *)(call)->ret;
	switch((call)->retsize) {
	case 2:
	  if ((call)->rettype == 'D')
	    *(double*)res = f0d;
	  else {
		res[1] = d1;
		res[0] = d0;
	  }
	  break;
	case 1:
	  if ((call)->rettype == 'F')
	    *(double*)res = f0f;
	  else
		res[0] = d0;
	  break;
	}
	}
}

#endif /* __m68k_linux_md_h */

