/*
 * parisc/common.h
 * Common configuration information for PA-RISC based machines.
 *
 * Copyright (c) 1996, 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * sysdepCallMethod is written by Pavel Roskin <pavel_roskin@geocities.com>
 */

#ifndef __parisc_common_h
#define __parisc_common_h

typedef jdouble d4int_f (int, int, int, int);
typedef jfloat f4int_f (int, int, int, int);
typedef jlong j4int_f (int, int, int, int);

#define NEED_STACK_ALIGN
#define STACK_ALIGN(p)  ((((unsigned long)(p)) & 15) ^ (unsigned long)(p))

#if NEED_sysdepCallMethod
/* FIXME: this doesn't work for parisc-linux, because on parisc-linux
 * there is no fparg relocation, so we cannot just pass everything in
 * general purpose registers
 */

#define sysdepCallMethod(CALL)						\
do {									\
  int argidx;								\
  int *sa;								\
  int *stackx;								\
  int st_reserve[(CALL)->nrargs];					\
  asm volatile ("ldo -36(%%r30),%0" : "=r" (sa) : "r" (st_reserve));	\
  for (argidx = 0, stackx = sa; argidx < (CALL)->nrargs; argidx++)	\
    switch ((CALL)->callsize[argidx])					\
      {									\
      case 1:								\
	*stackx-- = (CALL)->args[argidx].i;				\
	break;								\
      case 2:								\
	if (!((int)stackx & 4)) stackx--;				\
	*stackx-- = (int) ((CALL)->args[argidx].j);			\
	*stackx-- = (CALL)->args[argidx].i;				\
	argidx++;							\
      default:								\
	break;								\
      }									\
  if ((CALL)->rettype == 'F') {						\
    (CALL)->ret->f =							\
      ((f4int_f *) ((CALL)->function)) (sa[0], sa[-1], sa[-2], sa[-3]);	\
    break;								\
  }									\
  if ((CALL)->rettype == 'D') {						\
    (CALL)->ret->d =							\
      ((d4int_f *) ((CALL)->function)) (sa[0], sa[-1], sa[-2], sa[-3]);	\
    break;								\
  }									\
  (CALL)->ret->j =							\
    ((j4int_f *) ((CALL)->function)) (sa[0], sa[-1], sa[-2], sa[-3]);	\
} while (0);
#endif

/*
 * Do an atomic compare and exchange.  The address 'A' is checked against
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is sucessful, otherwise 0.
 *
 * pa doesn't have an atomic compare-and-exchange instruction. we use
 * a C version a la MIPS for now. Might not be SMP safe.
 */
#define COMPARE_AND_EXCHANGE(A,O,N)            \
({                                             \
    int ret = 0;                               \
    jthread_suspendall();                      \
                                               \
    if (*(A) == (O)) {                         \
       *(A) = (N);                             \
       ret = 1;                                \
    }                                          \
    jthread_unsuspendall();                    \
    ret;                                       \
})

#endif
