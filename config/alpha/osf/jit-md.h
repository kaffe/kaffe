/* Needs update, do not use */

/*
 * alpha/osf/jit-md.h
 * OSF/1 Alpha JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __alpha_osf_jit_md_h
#define __alpha_osf_jit_md_h

/**/
/* Include common information. */
/**/
#include "alpha/jit.h"

/**/
/* Extra exception handling information. */
/**/
#include <signal.h>


/**/
/* Exception handling information. */
/**/

/* Structure of exception frame on stack */
typedef struct _exceptionFrame {
        struct sigcontext sc;
} exceptionFrame;

void __alpha_osf_firstFrame (exceptionFrame *frame);
exceptionFrame * __alpha_osf_nextFrame (exceptionFrame *frame);

/* Native support for exception */

#define MD_JIT_EXCEPTION_INFO_LENGTH		(8 + 8 + 24)
#define MD_REGISTER_JIT_EXCEPTION_INFO(M,B,L)	__alpha_osf_register_jit_exc(M,B,L)
#define MD_UNREGISTER_JIT_EXCEPTION_INFO(M,B,L)	__alpha_osf_unregister_jit_exc(M,B,L)

void __alpha_osf_register_jit_exc (void *methblock, void *codebase, int codelen);
void __alpha_osf_unregister_jit_exc (void *methblock, void *codebase, int codelen);


/* Get the next frame in the chain */
#define NEXTFRAME(f)							\
	__alpha_osf_nextFrame (f)

/* Extract the PC from the given frame */
#define PCFRAME(f)		((f)->sc.sc_pc)
#define FPFRAME(f)		((f)->sc.sc_regs[15])

/* Get the first exception frame from a subroutine call */
#define FIRSTFRAME(f, o)						\
	__alpha_osf_firstFrame (&(f))

#if defined SA_SIGINFO
#undef SA_SIGINFO
#endif

#define EXCEPTIONPROTO							\
	int sig, int code, struct sigcontext *ctx

/* Get the first exception frame from a signal handler */
#define EXCEPTIONFRAME(f, c)						\
	(f).sc = *(c)
	/* __alpha_osf_firstFrame (&(f)) */

#endif
