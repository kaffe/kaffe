/*
 * sparc/common.h
 * Common SPARC configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_common_h
#define __sparc_common_h

#define	sysdepCallMethod(CALL) do {					\
  /* allocates variable arrays one word past the minimum stack frame */	\
  int extraargs[((CALL)->nrargs>6)?((CALL)->nrargs-6):0];		\
  switch((CALL)->nrargs) {						\
    register int o0 asm("o0");						\
    register int o1 asm("o1");						\
    register int o2 asm("o2");						\
    register int o3 asm("o3");						\
    register int o4 asm("o4");						\
    register int o5 asm("o5");						\
    register double f0d asm("f0");					\
    register float f0f asm("f0");					\
    int *res;								\
									\
  default:								\
    {									\
      int *args = extraargs-1;						\
      int argidx = 6;							\
      for(; argidx < (CALL)->nrargs; ++argidx, ++args) {		\
	*args = (CALL)->args[argidx].i;					\
      }									\
    }									\
  case 6:								\
    o5 = (CALL)->args[5].i;						\
  case 5:								\
    o4 = (CALL)->args[4].i;						\
  case 4:								\
    o3 = (CALL)->args[3].i;						\
  case 3:								\
    o2 = (CALL)->args[2].i;						\
  case 2:								\
    o1 = (CALL)->args[1].i;						\
  case 1:								\
  case 0:								\
    asm ("call %2,0\n							\
          ld %3, %0\n"							\
	: "=r" (o0), "=r" (o1)						\
	: "r" ((CALL)->function),					\
	  "m" ((CALL)->args?(CALL)->args[0].i:0),			\
	  "0" (o0), "1" (o1), "r" (o2), "r" (o3), "r" (o4), "r" (o5)	\
	: "g1", "g2", "g3", "g4", "o7", "cc"				\
	);								\
    if ((CALL)->retsize != 0)						\
      res = &(CALL)->ret->i;						\
    switch((CALL)->retsize) {						\
    case 2:								\
      if ((CALL)->rettype == 'D')					\
        *(double*)res = f0d;						\
      else {								\
        res[1] = o1;							\
        res[0] = o0;							\
      }									\
      break;								\
    case 1:								\
      if ((CALL)->rettype == 'F')					\
        *(float*)res = f0f;						\
      else								\
        *res = o0;							\
      break;								\
    case 0:								\
      break;								\
    default:								\
      ABORT();								\
      break;								\
    }									\
    break;								\
  }									\
} while (0)

#endif
