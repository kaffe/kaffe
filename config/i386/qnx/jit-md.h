/*
 * i386/qnx/jit-md.h
 * Qnx i386 JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_qnx_jit_md_h
#define __i386_qnx_jit_md_h

/**/
/* Include common information. */
/**/
#include "i386/jit.h"

/* In the QNX standard libraries, floats/doubles are not returned in
 * the 387, but in ax/dx (like 32/64 bit ints). In order for native
 * functions to be compatible, we must follow
 */

#undef HAVE_return_float
#undef HAVE_return_double
#undef HAVE_returnarg_float
#undef HAVE_returnarg_double

#define	HAVE_return_float		return_Rxx
#define	HAVE_return_double		returnl_Rxx
#define	HAVE_returnarg_float		returnarg_xxR
#define	HAVE_returnarg_double		returnargl_xxR

/**/
/* Extra exception handling information. */
/**/

#include <sys/types.h>

/* Function prototype for signal handlers */
#define	EXCEPTIONPROTO SIGNAL_ARGS(sig, ctx)

/* Get the first exception frame from a signal handler */
#define	EXCEPTIONFRAME(f, c) \
	(f).retbp = (c)->sc_bp; \
	(f).retpc = (c)->sc_ip + 1

#endif
