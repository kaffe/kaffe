/*
 * i386/bsdi/md.h
 * BSDI i386 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003, 2004
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_bsdi_md_h
#define __i386_bsdi_md_h

#include "i386/common.h"
#include "i386/threads.h"

#if defined(HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

/*
 * Redefine stack pointer offset.
 */
#undef	SP_OFFSET
#define	SP_OFFSET	2

#define SIGNAL_ARGS(sig, sc) int sig, int code, struct sigcontext* sc
#define SIGNAL_CONTEXT_POINTER(scp)  struct sigcontext* scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (sc)
#define SIGNAL_PC(scp) (scp)->sc_pc
#define STACK_POINTER(scp) (scp)->sc_sp

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif


/*
        layout of i386_jump_buf.
	Note that sp must be in the 3rd position to be compatible with 
	native bsdi jump buffer.
                unsigned long Ebp;
                unsigned long Edi;
                unsigned long Esp;
                unsigned long Esi;
                unsigned long Ebx;
                unsigned long Eip;
                unsigned long Edx;
*/
typedef long i386_jmp_buf[7];
int i386_setjmp(i386_jmp_buf buf);
int i386_longjmp(i386_jmp_buf buf, int rc);

#undef JTHREAD_SETJMP
#define JTHREAD_SETJMP(buf)       i386_setjmp((buf))
#undef JTHREAD_LONGJMP
#define JTHREAD_LONGJMP(buf, val) i386_longjmp((buf), (val))
#undef JTHREAD_JMPBUF
#define JTHREAD_JMPBUF            i386_jmp_buf

#include "kaffe-unix-stack.h"

#endif

