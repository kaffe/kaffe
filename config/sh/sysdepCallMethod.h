/*
 * sh/sysdepCallMethod.h
 * Common sysdepCallMethod for Super-H.
 *
 * Copyright (c) 2001
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *	Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#if ! defined(__sh_sysdepCallMethod_h)
#define __sh_sysdepCallMethod_h

#include "support.h"

static inline void sysdepCallMethod(callMethodInfo *call) ALWAYS_INLINE;

static inline void sysdepCallMethod(callMethodInfo *call)
{
  int extraargs[((call)->nrargs>4)?((call)->nrargs-4):0];
  switch((call)->nrargs) {
    register int r0 asm("r4");
    register int r1 asm("r5");
    register int r2 asm("r6");
    register int r3 asm("r7");
  default:
    {
      int *args = extraargs;
      int argidx = 4;
      if ((call)->callsize[3] == 2) args++;
      for(; argidx < (call)->nrargs; ++argidx) {
	if ((call)->callsize[argidx]) {
	  *args++ = (call)->args[argidx].i;
	  if ((call)->callsize[argidx] == 2)
	    *args++ = ((call)->args[argidx].j) >> 32;
	}
      }
    }
  case 4:
    if ((call)->callsize[3]) {
      r3 = (call)->args[3].i;
      if ((call)->callsize[3] == 2)
        *extraargs = ((call)->args[3].j) >> 32;
    }
  case 3:
    if ((call)->callsize[2]) {
      r2 = (call)->args[2].i;
      if ((call)->callsize[2] == 2)
        r3 = ((call)->args[2].j) >> 32;
    }
  case 2:
    if ((call)->callsize[1]) {
      r1 = (call)->args[1].i;
      if ((call)->callsize[1] == 2)
        r2 = ((call)->args[1].j) >> 32;
    }
  case 1:
    if ((call)->callsize[0]) {
      r0 = (call)->args[0].i;
      if ((call)->callsize[0] == 2)
        r1 = ((call)->args[0].j) >> 32;
    }
  case 0:
    asm volatile ("jsr @%2 ; nop ; mov r0,r4 ; mov r1,r5\n"
        : "=r" (r0), "=r" (r1)
	: "r" ((call)->function),
	  "0" (r0), "1" (r1), "r" (r2), "r" (r3)
	: "r14"
	);
    switch((call)->rettype) {
    case 'D':
    case 'J':
        (&(call)->ret->i)[1] = r1;
        /* follow through */
    default:
        (call)->ret->i = r0;
    }
    break;
  }
}

#endif /* ! defined(__sh_sysdepCallMethod_h) */
