/*
 * mips/irix/jit-md.h
 * MIPS Irix JIT configuration information.
 *
 * Copyright (c) 1996-1999 Transvirtual Technologies, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Written by Tim Wilkinson <tim@transvirtual.com>
 */

#ifndef __mips_irix_jit_md_h
#define __mips_irix_jit_md_h

/**/
/* Include common information. */
/**/
#include "mips/jit.h"

/*
 * newer Linux kernel actually implement SA_SIGINFO.
 * But we don't need it, so let's turn it off
 */
#if defined(SA_SIGINFO)
#undef SA_SIGINFO
#endif

/* Define the entry into a signal handler */
#define EXCEPTIONPROTO  int sig, int cause, struct sigcontext *ctx

/* Get the first exception frame from a signal handler */
#define MIPS_SP 29
#define MIPS_FP 30
#define EXCEPTIONFRAME(f, c) \
	(f).return_frame = (void *)(unsigned long)(c)->sc_regs[MIPS_FP]; \
	(f).return_sp    = (void *)(unsigned long)(c)->sc_regs[MIPS_SP]; \
	(f).return_pc = (void *)(unsigned long)(c)->sc_pc

#endif
