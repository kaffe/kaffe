/*
 * arm/linux/jit-md.h
 * ARM Linux JIT configuration information.
 *
 * Copyright (c) 1996, 1997, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __arm_riscos_jit_md_h
#define __arm_riscos_jit_md_h

/**/
/* Include common information. */
/**/
#include "arm/jit.h"

/**/
/* Extra exception handling information. */
/**/
#include <signal.h>

/* Allow simply to compile */
#define	EXCEPTIONPROTO							\
	int sig, void * /*struct sigcontext_struct*/ ctx

/* Do nothing for now */
#define EXCEPTIONFRAME(f, c)


