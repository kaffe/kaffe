/*
 * alpha/netbsd/jit-md.h
 * NetBSD Alpha JIT configuration information.
 *
 * Copyright (c) 2001
 *	Edouard G. Parmelan.  All rights reserved.
 *
 * Copyright (c) 2001
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __alpha_netbsd_jit_md_h
#define __alpha_netbsd_jit_md_h

/**/
/* Include common information. */
/**/
#include "alpha/jit.h"


/**/
/* Extra exception handling information. */
/**/

/* Function prototype for signal handlers */
#define	EXCEPTIONPROTO					\
	int sig, int code, struct sigcontext* ctx


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
