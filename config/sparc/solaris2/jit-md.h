/*
 * sparc/solaris2/jit-md.h
 * SPARC Solaris2 JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_solaris2_jit_md_h
#define __sparc_solaris2_jit_md_h

/**/
/* Include common information. */
/**/
#include "sparc/jit.h"

/* Function prototype for signal handlers */
#define	EXCEPTIONPROTO SIGNAL_ARGS(sig, ctx)

/* Get the first exception frame from a signal handler */
#define	EXCEPTIONFRAME(f, c)						\
	(f).retbp = (c)->uc_mcontext.gregs[REG_SP];			\
	(f).retpc = (c)->uc_mcontext.gregs[REG_PC]

#endif
