/*
 * sparc/uxpds/jit-md.h
 * SPARC Uxpds JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_uxpds_jit_md_h
#define __sparc_uxpds_jit_md_h

/**/
/* Include common information. */
/**/
#include "sparc/jit.h"

/**/
/* Extra exception handling information. */
/**/
#include <siginfo.h>
#include <ucontext.h>

/* Function prototype for signal handlers */
#define	EXCEPTIONPROTO							\
	int sig, siginfo_t* sip, ucontext_t* ctx

/* Get the first exception frame from a signal handler */
#define	EXCEPTIONFRAME(f, c)						\
	(f).retbp = (c)->uc_mcontext.gregs[REG_SP];			\
	(f).retpc = (c)->uc_mcontext.gregs[REG_PC]

#endif
