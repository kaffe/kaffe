/*
 * superh/common.h
 * Common Super-H configuration information.
 *
 * Copyright (c) 2001
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __superh_common_h
#define __superh_common_h

#include <stddef.h>

#define	sysdepCallMethod(CALL) do {				\
  int extraargs[((CALL)->nrargs>4)?((CALL)->nrargs-4):0];		\
  switch((CALL)->nrargs) {						\
    register int r0 asm("r4");						\
    register int r1 asm("r5");						\
    register int r2 asm("r6");						\
    register int r3 asm("r7");						\
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
      }								\
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
    asm volatile ("jsr @%2 ; nop ; mov r0,r4 ; mov r1,r5\n"		\
        : "=r" (r0), "=r" (r1)						\
	: "r" ((CALL)->function),					\
	  "0" (r0), "1" (r1), "r" (r2), "r" (r3) 			\
	: "r14"				 				\
	);								\
    switch((CALL)->rettype) {                                          \
    case 'D':                                                          \
    case 'J':                                                          \
        (&(CALL)->ret->i)[1] = r1;                                     \
        /* follow through */                                           \
    default:                                                           \
        (CALL)->ret->i = r0;                                           \
    }                                                                  \
    break;								\
  }									\
} while (0)

/*
 * Do an atomic compare and exchange.  The address 'A' is checked against
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is sucessful, otherwise 0.
 *
 * Copied from "config/mips/common.h".
 */
#define COMPARE_AND_EXCHANGE(A,O,N)		\
({						\
    int ret = 0;				\
    jthread_suspendall();			\
						\
    if (*(A) == (O)) {				\
	*(A) = (N);				\
	ret = 1;				\
    }						\
    jthread_unsuspendall();			\
    ret;					\
})

#endif
