/*
 * sparc/sunos4/jit-md.h
 * SPARC Sunos$ JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_sunos4_jit_md_h
#define __sparc_sunos4_jit_md_h

/**/
/* Include common information. */
/**/
#include "sparc/jit.h"

/**/
/* Extra exception handling information. */
/**/

/* Function prototype for signal handlers */
#define	EXCEPTIONPROTO							\
	int sig, int code, struct sigcontext* ctx, char* addr

/* Get the first exception frame from a signal handler */
#define	EXCEPTIONFRAME(f, c)						\
	(f).retbp = (c)->sc_sp;						\
	(f).retpc = (c)->sc_pc

#endif
