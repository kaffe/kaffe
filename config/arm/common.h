/*
 * arm32/common.h
 * Common arm32 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * Written by Godmar Back <gback@cs.utah.edu>
 */

#ifndef __arm32_common_h
#define __arm32_common_h

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

#define	sysdepCallMethod(CALL) do {					\
  int extraargs[((CALL)->nrargs>4)?((CALL)->nrargs-4):0];		\
  switch((CALL)->nrargs) {						\
    register int r0 asm("r0");						\
    register int r1 asm("r1");						\
    register int r2 asm("r2");						\
    register int r3 asm("r3");						\
    int *res;								\
  default:								\
    {									\
      int *args = extraargs;						\
      int argidx = 4; 							\
      if ((CALL)->callsize[3] == 2) args++;				\
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
    asm ("mov lr, pc\n							\
	  mov pc, %2\n"							\
        : "=r" (r0), "=r" (r1)						\
	: "r" ((CALL)->function),					\
	  "0" (r0), "1" (r1), "r" (r2), "r" (r3) 			\
	: "ip", "rfp", "sl", "fp", "lr" 				\
	);								\
    res = (int *)(CALL)->ret; 						\
    res[0] = r0; 							\
    res[1] = r1; 							\
    break;								\
  }									\
} while (0)								\

#endif
