/*
 * arm/sysdepCallMethod.h
 * Common arm sysdepCallNethod.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * Written by Godmar Back <gback@cs.utah.edu>
 */

#ifndef __arm_sysdepCallMethod_h
#define __arm_sysdepCallMethod_h

#include "support.h"

/*
 * The calling convention is such that the first four 32bit values are
 * passed in r0-r3, and the remainder goes on the stack.
 * Floating point values are passed in integer registers.
 *
 * This machine is a little endian machine, but double floating point 
 * values are stored in big endian *word* order.  Note that we do not
 * have to take this into account here.  It is a convention of the
 * software floating point libraries and the build tools.
 */
#if defined(NEED_sysdepCallMethod)
static inline void sysdepCallMethod(callMethodInfo *call) ALWAYS_INLINE;

static inline void sysdepCallMethod(callMethodInfo *call) {
  int extraargs[(call->nrargs>4)?(call->nrargs-4):0];
  switch(call->nrargs) {
    register int r0 asm("r0");
    register int r1 asm("r1");
    register int r2 asm("r2");
    register int r3 asm("r3");
#ifndef __SOFTFP__
    register double f0 asm("f0");
#endif

  default:
    {
      int *args = extraargs;
      int argidx = 4;
      for(; argidx < call->nrargs; ++argidx) {
        *args++ = call->args[argidx].i;
      }
    }
  case 4:
    r3 = call->args[3].i;
  case 3:
    r2 = call->args[2].i;
  case 2:
    r1 = call->args[1].i;
  case 1:
    r0 = call->args[0].i;
  case 0:
#ifdef __SOFTFP__
    asm volatile ("mov lr, pc\n"
"                  mov pc, %2\n"
                  : "=r" (r0), "=r" (r1)
                  : "r" (call->function),
                    "0" (r0), "1" (r1), "r" (r2), "r" (r3)
                  : "ip", "lr");
#else
    asm volatile ("mov lr, pc\n"
"                    mov pc, %3\n"
                  : "=r" (r0), "=r" (r1), "=f" (f0)
                  : "r" (call->function),
                    "0" (r0), "1" (r1), "r" (r2), "r" (r3)
                  : "ip", "lr");
#endif
    switch (call->rettype)
    {
    case 'V':
      break;

    case 'D':
#ifdef __SOFTFP__
      (&call->ret->i)[1] = r1;
      (&call->ret->i)[0] = r0;
#else
      call->ret->d = (double) f0;
#endif
      break;

    case 'F':
#ifdef __SOFTFP__
      call->ret->i = r0;
#else
      call->ret->f = (float) f0;
#endif
      break;
 
    case 'J':
      (&call->ret->i)[1] = r1;
      (&call->ret->i)[0] = r0;
      break;

    default:
      call->ret->i = r0;
      break;
    }
  }
}
#endif /* defined(NEED_sysdepCallMethod) */

#endif /* __arm_sysdepCallMethod_h */
