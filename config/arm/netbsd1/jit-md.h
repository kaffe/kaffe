/*
 * arm/netbsd1/jit-md.h
 * ARM NetBSD JIT configuration information.
 *
 * Copyright (c) 1996, 1997, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __arm_netbsd1_jit_md_h
#define __arm_netbsd1_jit_md_h

/* We have no floating point or kernel emulation */
#define HAVE_NO_FLOATING_POINT  1

/**/
/* Include common information. */
/**/
#include "arm/jit.h"

#define EXCEPTIONPROTO  int sig, int code, struct sigcontext* ctx

#if defined(HAVE_SYS_UCONTEXT_H)
#define EXCEPTIONFRAME(F, C)                    \
	(F).retfp = ((ucontext_t *)(C))->uc_mcontext.__gregs[_REG_FP]; \
	(F).retpc = _UC_MACHINE_PC(((ucontext_t *)(C)))
#else
/* Not yet tested */
#define EXCEPTIONFRAME(F, C)                    \
	(F).retfp = (void*)(C)->sc_r11;		\
	(F).retsp = (void*)(C)->sc_usr_sp;	\
	(F).retpc = (void*)(C)->sc_pc;
#endif

#endif
