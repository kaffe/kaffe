/*    
 * arm/riscos/md.h
 * RISC OS arm configuration information.
 *
 * Copyright (c) 2003
 *   Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file.
 */
 
#ifndef __arm_riscos_md_h
#define __arm_riscos_md_h
 
#include "arm/common.h"
#include "arm/threads.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#include "support.h"

/* RISC OS requires a little initialisation */
extern void init_md(void);
#define INIT_MD()       init_md()

/* Define CREATE_NULLPOINTER_CHECKS in md.h when your machine cannot use the
 * MMU for detecting null pointer accesses
 *
 * Actually, when debugging, it would be nice to don't catch unexpected
 * exception, so...
 */

#define CREATE_NULLPOINTER_CHECKS

#undef SP_OFFSET
#undef FP_OFFSET

#define SP_OFFSET               22
#define FP_OFFSET               21

static inline void sysdepCallMethod(callMethodInfo *call) 
{
  int i=(call)->nrargs-1;
  int extraargs[((call)->nrargs>4)?((call)->nrargs-4):0];
  switch((call)->nrargs) {
    register int r0 asm("r0");
    register int r1 asm("r1");
    register int r2 asm("r2");
    register int r3 asm("r3");
    register double f0 asm("f0");
  default:
    {
      int *args = extraargs;
      int argidx = 4;
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
    for (i=(call)->nrargs-1; i>3; i--) {
      asm volatile("stmdb __sp!, {%0}\n"
	:
	: "r" ((call)->args[i].i)
	: "sp");
    }
    asm ("mov __lr, __pc\n"
	 "mov __pc, %3\n"
        : "=r" (r0), "=r" (r1), "=f" (f0)
	: "r" ((call)->function),
	  "0" (r0), "1" (r1), "r" (r2), "r" (r3)
	: "sp", "sl", "lr", "cc"
	);
    for (i=(call)->nrargs-1; i>3; i--) {
      asm volatile("add __sp, __sp, #4\n"
	: : : "sp" );
    }
    switch((call)->rettype)
    {
    case 'D':
        asm("stfd %1,%0" : "=m" ((call)->ret->d) : "f" (f0));
	break;
    case 'F':
	asm("stfs %1,%0" : "=m" ((call)->ret->f) : "f" (f0));
	break;
    case 'L':
	(call)->ret->l = r0;
	break;
    case 'J':
	(&(call)->ret->i)[1] = r1;
	/* follow through */
    default:
	(call)->ret->i = r0;
    }
    break;
  }
}

#endif
