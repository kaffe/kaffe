/*
 * mips/linux/md.h
 * Linux MIPS configuration information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __mips_linux_md_h
#define __mips_linux_md_h

#include "mips/common.h"
#include "mips/sysdepCallMethod.h"
#include "mips/threads.h"

#undef	SP_OFFSET
#undef	FP_OFFSET

#define	SP_OFFSET	1

#ifdef PS2LINUX
#define FP_OFFSET	36
#else
#define	FP_OFFSET	10
#endif

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/**/
/* Signal handling */
/**/
#include <signal.h>

#include "sigcontextinfo.h"

#define SIGNAL_ARGS(sig, sc) int sig, int dummy, struct sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext * scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (sc)
#define SIGNAL_PC(scp) (GET_PC(scp))
#define STACK_POINTER(scp) (GET_STACK(scp))

#endif
