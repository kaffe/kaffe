/* Needs update, do not use */

/*
 * sparc/nextstep3/jit-md.h
 * NEXTSTEP SPARC JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_nextstep3_jit_md_h
#define __sparc_nextstep3_jit_md_h

/**/
/* Include common information. */
/**/
#include "sparc/jit.h"

/**/
/* Extra exception handling information. */
/**/

/* Function prototype for signal handlers */
#define	EXCEPTIONPROTO SIGNAL_ARGS(sig, ctx)

/* Get the first exception frame from a signal handler */
#define	EXCEPTIONFRAME(f, c)						\
	(f).retbp = c->sc_ebp;					\
	(f).retpc = c->sc_eip + 1

#endif
