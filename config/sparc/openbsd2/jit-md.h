/*
 * sparc/openbsd2/jit-md.h
 * OpenBSD Sparc JIT configuration information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_openbsd2_jit_md_h
#define __sparc_openbsd2_jit_md_h

/**/
/* Include common information. */
/**/
#include "sparc/jit.h"

#undef	FLUSH_DCACHE
#define	FLUSH_DCACHE(beg, end)						\
	do {								\
		int *_p = (int *)(beg), *_e = (int *)(end);		\
		while (_p < _e) {					\
			asm volatile("iflush %0+0" : : "r"(_p++));	\
		}							\
	} while (0)

/**/
/* Extra exception handling information. */
/**/

/* Function prototype for signal handlers */
#define	EXCEPTIONPROTO							\
	int sig, int code, struct sigcontext* ctx

/* Get the first exception frame from a signal handler */
#define	EXCEPTIONFRAME(f, c)						\
	(f).retbp = (c)->sc_sp;						\
	(f).retpc = (c)->sc_pc

#endif
