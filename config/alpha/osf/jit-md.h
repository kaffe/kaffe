/*
 * alpha/osf/jit-md.h
 * OSF/1 Alpha JIT configuration information.
 *
 * Copyright (c) 1999, 2000, 2001
 *	Edouard G. Parmelan.  All rights reserved.
 *
 * Copyright (c) 1996, 1997, 1999, 2000, 2001
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

#if defined SA_SIGINFO
#undef SA_SIGINFO
#endif

#define EXCEPTIONPROTO					\
	int sig, int code, struct sigcontext *ctx


#if defined(USE_LIBEXC)
/* Structure of exception frame on stack */
typedef struct _exceptionFrame {
        struct sigcontext sc;
} exceptionFrame;

void __alpha_osf_firstFrame (exceptionFrame *frame);
exceptionFrame * __alpha_osf_nextFrame (exceptionFrame *frame);

/* Native support for exception */

#define MD_JIT_EXCEPTION_INFO_LENGTH		(8 + 8 + 24)
#define MD_REGISTER_JIT_EXCEPTION_INFO(M,B,E)	__alpha_osf_register_jit_exc(M,B,E)
#define MD_UNREGISTER_JIT_EXCEPTION_INFO(M,B,E)	__alpha_osf_unregister_jit_exc(M,B,E)

void __alpha_osf_register_jit_exc (void *methblock, void *codebase, void *codeend);
void __alpha_osf_unregister_jit_exc (void *methblock, void *codebase, void *codeend);


/* Get the next frame in the chain */
#define NEXTFRAME(f)							\
	__alpha_osf_nextFrame (f)

/* Extract PC, FP and SP from the given frame */
#define PCFRAME(f)		((f)->sc.sc_pc)
#define FPFRAME(f)		((f)->sc.sc_regs[15])
#define SPFRAME(f)		((f)->sc.sc_regs[30])

/* Get the first exception frame from a subroutine call */
#define FIRSTFRAME(f, o)						\
	__alpha_osf_firstFrame (&(f))

/* Get the first exception frame from a signal handler */
#define EXCEPTIONFRAME(f, c)						\
	(f).sc = *(c)

#else

/* Structure of exception frame on stack */
typedef struct _exceptionFrame {
	char *pc;
	char *sp;
	char *fp;
} exceptionFrame;

int __alpha_ra (int *pc);
exceptionFrame *__alpha_nextFrame (exceptionFrame *frame);

/* Extract PC, FP and SP from the given frame */
#define PCFRAME(f)	((f)->pc)
#define SPFRAME(f)	((f)->sp)
#define FPFRAME(f)	((f)->fp)

/* Get the first exception frame from a subroutine call */
#define FIRSTFRAME(f, o)			\
	(f).sp = 0;				\
	__alpha_nextFrame(&f)

/* Get the next frame in the chain */
#define NEXTFRAME(f)				\
	__alpha_nextFrame(f)

/* Get the first exception frame from a signal handler */
#define EXCEPTIONFRAME(f, c)				\
	(f).pc = (__alpha_ra ((c)->sc_pc) == -1)	\
	    ? (c)->sc_pc				\
	    : (c)->sc_regs[__alpha_ra ((c)->sc_pc)];	\
	(f).sp = (c)->sc_regs[30];			\
	(f).fp = (c)->sc_regs[15]

#endif

#endif
