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
