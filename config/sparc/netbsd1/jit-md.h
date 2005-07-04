/*
 * sparc/netbsd/jit-md.h
 * NetBSD sparc JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2005
 *      The kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_netbsd_jit_md_h
#define __sparc_netbsd_jit_md_h

/* mcontext.h gives us macros to access ucontext/mcontext */
#include <sys/ucontext.h>

/**/
/* Include common information. */
/**/
#include "sparc/jit.h"

/**/
/* Extra exception handling information. */
/**/

/* Function prototype for signal handlers */
#define EXCEPTIONPROTO                                                  \
        int sig, int code, ucontext_t* ctx

/* Get the first exception frame from a signal handler */
#define EXCEPTIONFRAME(f, c)                                            \
        (f).retbp = _UC_MACHINE_SP(((ucontext_t *)(c)));                \
        (f).retpc = _UC_MACHINE_PC(((ucontext_t *)(c)))



#endif /* include guard */
