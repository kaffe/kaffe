/*
 * i386/freebsd2/md.h
 * FreeBSD i386 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_freebsd2_md_h
#define __i386_freebsd2_md_h

#include "i386/common.h"
#include "i386/sysdepCallMethod.h"
#include "i386/threads.h"

/*
 * Redefine stack pointer offset.
 */
#undef	SP_OFFSET
#define	SP_OFFSET	2

/*
 * Redefine the base pointer offset in a jmpbuf.
 */
#undef  BP_OFFSET
#define BP_OFFSET	3

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
#define SIGNAL_ARGS(sig, sc) int sig, int __code, struct sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext *scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (sc)
#define SIGNAL_PC(scp) (scp)->sc_pc

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#endif
