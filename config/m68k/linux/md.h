/*
 * m68k/linux/md.h
 * Linux/m68k configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __m68k_linux_md_h
#define __m68k_linux_md_h

#include "m68k/common.h"
#include "m68k/threads.h"
#include "support.h"
#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#if defined(HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

/*
 * Alignment in structure is 2 bytes packed.
 */
#define ALIGNMENT_OF_SIZE(S)    (((S>1)?2:1))

/*
 * Stack offset.
 * This is the offset into the setjmp buffer where the stack pointer is
 * stored.
 */
#define SP_OFFSET	14

/**/
/* Extra exception handling information. */
/**/
#include <signal.h>

#include "sigcontextinfo.h"

#define SIGNAL_ARGS(sig, sc) int sig, int code, struct sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext *scp
#define GET_SIGNAL_CONTEXT_POINTER(scp) (scp)
#define SIGNAL_PC(scp) (GET_PC(scp))
#define STACK_POINTER(scp) (GET_STACK(scp))

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* Linux requires a little initialisation */
extern void init_md(void);
#define	INIT_MD()	init_md()

#include "kaffe-unix-stack.h"

/*
 * sysdepCallMethod supports:
 *
 *	Linux version
 */
static inline void sysdepCallMethod(callMethodInfo *call) ALWAYS_INLINE;

static inline void sysdepCallMethod(callMethodInfo *call)
{
  int extraargs[call->nrargs];
  register int d0 asm ("d0");
  register int d1 asm ("d1");
  register double f0d asm ("fp0");
  register float f0f asm ("fp0");
  int *res;
  int *args = extraargs;
  int argidx;

  for(argidx = 0; argidx < call->nrargs; ++argidx) {
    if (call->callsize[argidx])	
      *args++ = call->args[argidx].i;
    else
      *args++ = call->args[argidx-1].j;
  }

  asm volatile ("jsr	%2@\n"				
		: "=r" (d0), "=r" (d1)					
		: "a" (call->function)
		: "cc", "memory");
  
  if (call->retsize != 0) {
    res = (int *)call->ret;

    switch(call->retsize) {
    case 2:
      if ((call)->rettype == 'D')
	*(double*)res = f0d;
      else {	      
	res[1] = d1;
	res[0] = d0;
      }
      break;
    case 1:
      if ((call)->rettype == 'F')
	*(float*)res = f0f;
      else
	res[0] = d0;
      break;
    }
  }
}

#endif /* __m68k_linux_md_h */
