/*
 * i386/qnx/md.h
 * QNX i386 configuration information.
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

#ifndef __i386_qnx_md_h
#define __i386_qnx_md_h

#if !defined(HAVE_GETPAGESIZE)
#undef getpagesize()
#define cdecl
#include <sys/mman.h>
#define       getpagesize()   PAGESIZE
#endif
/* see notes below for why we don't NEED_sysdepCallMethod */
#undef  NEED_sysdepCallMethod

#include "i386/common.h"
#include "i386/threads.h"
#include "support.h"

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
#define	SP_OFFSET	7

/* 
 * The contents of a QNX signal context was posted to comp.os.qnx on 
 * Oct 6, 1997 by Steve McPolin, QNX Software Systems. 
 */

struct _sigcontext {
        ulong_t   sc_mask;
        ulong_t   sc_gs:16,:16; /* register set at fault time */
        ulong_t   sc_fs:16,:16;
        ulong_t   sc_es:16,:16;
        ulong_t   sc_ds:16,:16;
        ulong_t   sc_di;
        ulong_t   sc_si;
        ulong_t   sc_bp;
        ulong_t   :32;          /* hole from pushad */
        ulong_t   sc_bx;
        ulong_t   sc_dx;
        ulong_t   sc_cx;
        ulong_t   sc_ax;
        ulong_t   sc_ip;
        ulong_t   sc_cs:16, :16;
        ulong_t   sc_fl;
        ulong_t   sc_sp;
        ulong_t   sc_ss:16, :16;
        ulong_t   sc_info;       /* fault specific info */
        ushort_t  sc_errc;       /* error code pushed by processor */
        uchar_t   sc_fault;      /* actual fault # */
        uchar_t   sc_flags;      /* signal handler flags: */
};

#define SIGNAL_ARGS(sig, sc) int sig, struct _sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp) struct _sigcontext *scp
#define GET_SIGNAL_CONTEXT_POINTER(scp) (scp)
#define SIGNAL_PC(scp) ((scp)->sc_ip)
#define STACK_POINTER(scp) ((scp)->sc_sp)

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* QNX requires a little initialisation */
extern void init_md(void);
#define       INIT_MD()       init_md()

/*
 * QNX doesn't do function calls in the "standard" way, specifically it
 * returns floats and doubles in registers rather than in the floating
 * point unit.  We must make some changes to the sysdepCallMethod to
 * handle this.
 */
static inline void sysdepCallMethod(callMethodInfo *call) 
{
	asm volatile ("	\n"
"1:			\n"
"		cmpl $0,%0 \n"
"		je 3f \n"
"		decl %0	\n"
"		cmpb $0,(%2,%0)	\n"
"		je 1b \n"
"		cmpb $1,(%2,%0)	\n"
"		je 2f \n"
"		pushl 4(%1,%0,8) \n"
"2:		\n"
"		pushl (%1,%0,8)	\n"
"		jmpl 1b	\n"
"3:		\n"
"		call *%3 \n"
"		movl %5,%%ebx \n"
"		movb %4,%%cl \n"
"		movl %%eax,(%%ebx) \n"
"		cmpb $0x44,%%cl \n"
"		je 4f \n"
"		cmpb $0x4a,%%cl	\n"
"		jne 5f \n"
"4:		\n"
"		movl %%edx,4(%%ebx) \n"
"5:		\n"
"	" :
	  : "r" ((call)->nrargs),
	    "r" ((call)->args),
	    "r" ((call)->callsize),
	    "m" ((call)->function),
	    "m" ((call)->rettype),
	    "m" ((call)->ret)
	  : "eax", "ebx", "ecx", "edx", "edi", "esi", "cc", "memory");
	asm volatile ("	\n"
"		subl %0,%%esp \n"
"	" : : "r" ((call)->argsize * sizeof(jint)) : "cc");
}


#include "kaffe-unix-stack.h"

#endif
