/*
 * sparc/linux/jit-md.h
 * Linux SPARC JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_linux_jit_md_h
#define __sparc_linux_jit_md_h

/**/
/* Include common information. */
/**/
#include "sparc/jit.h"

/**/
/* Extra exception handling information. */
/**/
#if defined(HAVE_FEATURES_H)
#include <features.h>
#endif 
#if defined(HAVE_SIGCONTEXT_H)
#include <sigcontext.h> 
#endif

/* Function prototype for signal handlers */
#define	EXCEPTIONPROTO							\
	int sig, struct sigcontext* ctx

/* Get the first exception frame from a signal handler */
#define	EXCEPTIONFRAME(f, c)						\
	(f).retbp = GET_STACK(c);			\
	(f).retpc = GET_PC(c)

#endif
