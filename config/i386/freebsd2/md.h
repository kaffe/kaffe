/*
 * i386/freebsd2/md.h
 * FreeBSD i386 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_freebsd2_md_h
#define __i386_freebsd2_md_h

#include "i386/common.h"
#include "i386/threads.h"

/*
 * Redefine stack pointer offset.
 */
#undef	SP_OFFSET
#define	SP_OFFSET	2

/*
 * FreeBSD does not save the floating point registers in the signal
 * context, so we must do it ourselves.
 */
/* Size of buffer in bytes */
#define	SAVED_FP_SIZE	108		
#define	SAVE_FP(fdata)			\
	asm("fsave %0": :"m"(*fdata))
#define	LOAD_FP(fdata)			\
	asm("frstor %0": :"m"(*fdata))

/* Define signal context macros for xprofiling */
#define SIGNAL_ARGS(sig, sc) int sig, int __code, struct sigcontext *##sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext *##scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (sc)
#define SIGNAL_PC(scp) (scp)->sc_pc

int kaffeStdProfRate(void);
#define KAFFE_STD_PROF_RATE \
int kaffeStdProfRate(void) \
{ \
	struct clockinfo clockinfo; \
	int retval = 0; \
	size_t size; \
	int mib[2]; \
	\
	size = sizeof(clockinfo); \
	mib[0] = CTL_KERN; \
	mib[1] = KERN_CLOCKRATE; \
	if (sysctl(mib, 2, &clockinfo, &size, NULL, 0) < 0) { \
	} else if (clockinfo.profhz == 0) { \
		retval = clockinfo.hz; \
	} else { \
		retval = clockinfo.profhz; \
	} \
	return( retval ); \
}

/* Override the standard mcount with the xprofiling one */
#include <machine/profile.h>

#define _KAFFE_OVERRIDE_MCOUNT_DEF _MCOUNT_DECL __P((uintfptr_t frompc, uintfptr_t selfpc))
#define _KAFFE_OVERRIDE_MCOUNT MCOUNT

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#endif
