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
 * sysdepCallMethod is written by Pavel Roskin <pavel.roskin@ecsoft.co.uk>
 */

#ifndef __parisc_common_h
#define __parisc_common_h

#define smart_args(idx) (idx<=3 ? ra[idx] : sa[-idx])

typedef jdouble d4int_f (int, int, int, int);
typedef jfloat f4int_f (int, int, int, int);
typedef jlong j4int_f (int, int, int, int);

#define sysdepCallMethod(CALL)						\
do {									\
  int argidx;								\
  int ra[4];								\
  int *sa;								\
  jlong jres;								\
  jfloat fres;								\
  jdouble dres;								\
  /* Is it possible to get stack pointer without asm statement? */	\
asm ("ldo -36(%%r30),%0":"=r" (sa));					\
  /* FIXME: we must make sure that stack has sufficient size */		\
  /* for all arguments!!! Maybe __builtin_alloca ? */			\
  for (argidx = 0; argidx < (CALL)->nrargs; argidx++)			\
    switch ((CALL)->callsize[argidx])					\
      {									\
      case 1:								\
	smart_args (argidx) = (CALL)->args[argidx].i;			\
	break;								\
      case 2:								\
	smart_args (argidx) = (int) ((CALL)->args[argidx].j);		\
	smart_args (argidx + 1) = (CALL)->args[argidx].i;		\
	argidx++;							\
      default:								\
	break;								\
      }									\
  /* Find a better way without typedef */				\
  switch ((CALL)->rettype)						\
    {									\
    case 'F':								\
      fres = ((f4int_f *) ((CALL)->function)) (ra[0], ra[1], ra[2], ra[3]);\
      break;								\
    case 'D':								\
      dres = ((d4int_f *) ((CALL)->function)) (ra[0], ra[1], ra[2], ra[3]);\
      break;								\
    default:								\
      jres = ((j4int_f *) ((CALL)->function)) (ra[0], ra[1], ra[2], ra[3]);\
      break;								\
    }									\
  switch ((CALL)->retsize)						\
    {									\
    case 1:								\
      if ((CALL)->rettype == 'F')					\
	(CALL)->ret->f = fres;						\
      else								\
	(CALL)->ret->j = jres;						\
      break;								\
    case 2:								\
      if ((CALL)->rettype == 'D')					\
	(CALL)->ret->d = dres;						\
      else								\
	(CALL)->ret->j = jres;						\
      break;								\
    default:								\
      break;								\
    }									\
} while (0);

#endif
