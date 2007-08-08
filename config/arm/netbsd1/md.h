/*
 * arm/netbsd1/md.h
 * NetBSD1.x/arm32 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __arm_netbsd1_md_h
#define __arm_netbsd1_md_h

#include "arm/common.h"
#include "arm/sysdepCallMethod.h"
#include "arm/threads.h"

/**/
/* Extra exception handling information. */
/**/
#include <signal.h>

#define	SIGNAL_ARGS(sig, sc) int sig, int code, struct sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext *scp
#define GET_SIGNAL_CONTEXT_POINTER(scp) (scp)
#define SIGNAL_PC(scp) ((uintp)(scp)->sc_pc)

/* #include "kaffe-unix-stack.h" */

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#endif
