/*
 * mips/netbsd1/md.h
 * NetBSD1 MIPS configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __mips_netbsd1_md_h
#define __mips_netbsd1_md_h

#include "mips/common.h"
#include "mips/threads.h"

/*
 * Stack offset.
 * This is the offset into the setjmp buffer where the stack pointer is
 * stored.  This may be different with different OSes.
 * NetBSD/mips needs not to define FP_OFFSET.
 */
#undef SP_OFFSET
#undef FP_OFFSET
#define SP_OFFSET	32


/*
 * System dependent call method routine definition.
 * We use 'sysdepCallMethod' (defined in md.c) for NetBSD/mips.
 */
extern void sysdepCallMethod(callMethodInfo *call);

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/**/
/* Signal handling */
/**/
#include <signal.h>

/* Define the entry into a signal handler */
#define	EXCEPTIONPROTO							\
	int sig, int code, struct sigcontext* ctx

/* Get the first exception frame from a signal handler */
#define MIPS_FP 30
#define EXCEPTIONFRAME(f, c) \
        (f).return_frame = (void*)(c).sc_regs[MIPS_FP]; \
        (f).return_pc = (void*)(c).sc_pc


#endif
