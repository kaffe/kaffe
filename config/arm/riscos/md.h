/*    
 * arm/riscos/md.h
 * RISC OS arm configuration information.
 */
 
#ifndef __arm_riscos_md_h
#define __arm_riscos_md_h
 
#include "arm/common.h"
#include "arm/threads.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

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

/* Override the general ARM sysdepCallMethod */
#undef sysdepCallMethod

#define	sysdepCallMethod(CALL) do {					\
  int i=(CALL)->nrargs-1;						\
  int extraargs[((CALL)->nrargs>4)?((CALL)->nrargs-4):0];		\
  switch((CALL)->nrargs) {						\
    register int r0 asm("r0");						\
    register int r1 asm("r1");						\
    register int r2 asm("r2");						\
    register int r3 asm("r3");						\
    register double f0 asm("f0");                                       \
  default:								\
    {									\
      int *args = extraargs;						\
      int argidx = 4; 							\
      for(; argidx < (CALL)->nrargs; ++argidx) {			\
	if ((CALL)->callsize[argidx]) { 				\
	  *args++ = (CALL)->args[argidx].i;				\
	  if ((CALL)->callsize[argidx] == 2) 				\
	    *args++ = ((CALL)->args[argidx].j) >> 32;			\
	} 								\
      }									\
    }									\
  case 4:								\
    if ((CALL)->callsize[3]) {						\
      r3 = (CALL)->args[3].i;						\
      if ((CALL)->callsize[3] == 2)					\
        *extraargs = ((CALL)->args[3].j) >> 32; 			\
    }									\
  case 3:								\
    if ((CALL)->callsize[2]) {						\
      r2 = (CALL)->args[2].i;						\
      if ((CALL)->callsize[2] == 2)					\
        r3 = ((CALL)->args[2].j) >> 32; 				\
    }									\
  case 2:								\
    if ((CALL)->callsize[1]) {						\
      r1 = (CALL)->args[1].i;						\
      if ((CALL)->callsize[1] == 2)					\
        r2 = ((CALL)->args[1].j) >> 32; 				\
    }									\
  case 1:								\
    if ((CALL)->callsize[0]) {						\
      r0 = (CALL)->args[0].i;						\
      if ((CALL)->callsize[0] == 2)					\
        r1 = ((CALL)->args[0].j) >> 32; 				\
    }									\
  case 0:								\
    for (i=(CALL)->nrargs-1; i>3; i--) {				\
      asm volatile("stmdb __sp!, {%0}\n"				\
	: 								\
	: "r" ((CALL)->args[i].i)					\
	: "sp");							\
    }									\
    asm ("mov __lr, __pc\n"						\
	 "mov __pc, %3\n"						\
        : "=r" (r0), "=r" (r1), "=f" (f0)				\
	: "r" ((CALL)->function),					\
	  "0" (r0), "1" (r1), "r" (r2), "r" (r3)			\
	: "sp", "sl", "lr", "cc"					\
	);								\
    for (i=(CALL)->nrargs-1; i>3; i--) {				\
      asm volatile("add __sp, __sp, #4\n"				\
	: : : "sp" );							\
    }									\
    switch((CALL)->rettype)    						\
    {									\
    case 'D':								\
        asm("stfd %1,%0" : "=m" ((CALL)->ret->d) : "f" (f0));		\
	break;								\
    case 'F':								\
	asm("stfs %1,%0" : "=m" ((CALL)->ret->f) : "f" (f0));		\
	break;								\
    case 'L':								\
	(CALL)->ret->l = r0;						\
	break;								\
    case 'J':								\
	(&(CALL)->ret->i)[1] = r1;					\
	/* follow through */						\
    default:								\
	(CALL)->ret->i = r0;						\
    }									\
    break;								\
  }									\
} while (0)

#endif
