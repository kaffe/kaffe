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

/**/
/* Include common information. */
/**/
#include "arm/jit.h"

/* We have no floating point or kernel emulation */
#define HAVE_NO_FLOATING_POINT  1


#define EXCEPTIONPROTO  int sig, int code, struct sigcontext* ctx

#define EXCEPTIONFRAME(F, C)                    \
	(F).retfp = (void*)(C)->sc_r11;		\
	(F).retsp = (void*)(C)->sc_usr_sp;	\
	(F).retpc = (void*)(C)->sc_pc;

#endif
