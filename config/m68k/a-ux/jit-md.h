/*
 * m68k/aux/jit-md.h
 * A/UX m68k JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __m68k_aux_jit_md_h
#define __m68k_aux_jit_md_h

/**/
/* Include common information. */
/**/
#include "m68k/jit.h"

/**/
/* Define where addresses are returned.  */
/**/

#define M68K_RETURN_REF		8	/* a0 */

/**/
/* Extra exception handling information. */
/**/
#include <signal.h>

#define	EXCEPTIONPROTO SIGNAL_ARGS(sig, ctx)

/* Get the first exception frame from a signal handler */
#define	EXCEPTIONFRAME(f, c)						\
	do {								\
		(f).retfp = (uintp)__builtin_frame_address(1);		\
		(f).retpc = (uintp)(c)->sc_pc;				\
	} while (0)

#endif
