/*
 * m68k/netbsd1/md.h
 * netbsd/m68k specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * Written by Kiyo Inaba <inaba@src.ricoh.co.jp>, 1998;
 */

#ifndef __m68k_netbsd1_md_h
#define __m68k_netbsd1_md_h

#include "m68k/common.h"
#include "m68k/threads.h"
#include "m68k/sysdepCallMethod.h"

/*
 * Redefine stack pointer offset.
 */
#undef  SP_OFFSET
#define SP_OFFSET       2

/**/
/* Extra exception handling information. */
/**/
#include <signal.h>

#define	SIGNAL_ARGS(sig, sc) int sig, int code, struct sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext *scp
#define GET_SIGNAL_CONTEXT_POINTER(scp) (scp)
#define SIGNAL_PC(scp) ((uintp)(scp)->sc_pc)

#include "kaffe-unix-stack.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#endif
