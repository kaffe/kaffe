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

/* 
 * The contents of a QNX signal context was posted to comp.os.qnx on 
 * Oct 6, 1997 by Steve McPolin, QNX Software Systems. 
 */

struct _sigcontext {
        ulong_t   sc_mask;
        ulong_t   sc_gs:16,:16; /* register set at fault time */
        ulong_t   sc_fs:16,:16;
        ulong_t   sc_es:16,:16;
        ulong_t   sc_ds:16,:16;
        ulong_t   sc_di;
        ulong_t   sc_si;
        ulong_t   sc_bp;
        ulong_t   :32;          /* hole from pushad */
        ulong_t   sc_bx;
        ulong_t   sc_dx;
        ulong_t   sc_cx;
        ulong_t   sc_ax;
        ulong_t   sc_ip;
        ulong_t   sc_cs:16, :16;
        ulong_t   sc_fl;
        ulong_t   sc_sp;
        ulong_t   sc_ss:16, :16;
        ulong_t   sc_info;       /* fault specific info */
        ushort_t  sc_errc;       /* error code pushed by processor */
        uchar_t   sc_fault;      /* actual fault # */
        uchar_t   sc_flags;      /* signal handler flags: */
};

/* Function prototype for signal handlers */
#define	EXCEPTIONPROTO \
	int sig, struct _sigcontext *ctx

/* Get the first exception frame from a signal handler */
#define	EXCEPTIONFRAME(f, c) \
	(f).retbp = (c)->sc_bp; \
	(f).retpc = (c)->sc_ip + 1

#endif
