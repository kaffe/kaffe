/*
 * parisc/sysdepCallMethod.h
 * Common configuration information for PA-RISC based machines.
 *
 * Copyright (c) 1996, 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * sysdepCallMethod is written by Pavel Roskin <pavel_roskin@geocities.com>
 */

#ifndef __parisc_sysdepCallMethod_h
#define __parisc_sysdepCallMethod_h

#include "support.h"

typedef jdouble d4int_f (int, int, int, int);
typedef jfloat f4int_f (int, int, int, int);
typedef jlong j4int_f (int, int, int, int);

#if defined(NEED_sysdepCallMethod)
/* FIXME: this doesn't work for parisc-linux, because on parisc-linux
 * there is no fparg relocation, so we cannot just pass everything in
 * general purpose registers
 */

static inline void sysdepCallMethod(callMethodInfo *call) ALWAYS_INLINE;

static inline void sysdepCallMethod(callMethodInfo *call)
{
  int argidx;
  int *sa;
  int *stackx;
  int st_reserve[(call)->nrargs];
  asm volatile ("ldo -36(%%r30),%0" : "=r" (sa) : "r" (st_reserve));

  for (argidx = 0, stackx = sa; argidx < (call)->nrargs; argidx++) {
    switch ((call)->callsize[argidx])
      {
      case 1:
	*stackx-- = (call)->args[argidx].i;
	break;
      case 2:
	if (!((int)stackx & 4)) stackx--;
	*stackx-- = (int) ((call)->args[argidx].j);
	*stackx-- = (call)->args[argidx].i;
	argidx++;
	break;
      };
  }

  if ((call)->rettype == 'F') {
    (call)->ret->f =
      ((f4int_f *) ((call)->function)) (sa[0], sa[-1], sa[-2], sa[-3]);
    return;
  }
  if ((call)->rettype == 'D') {
    (call)->ret->d =
      ((d4int_f *) ((call)->function)) (sa[0], sa[-1], sa[-2], sa[-3]);
    return;
  }
  (call)->ret->j =
    ((j4int_f *) ((call)->function)) (sa[0], sa[-1], sa[-2], sa[-3]);
}

#endif /* defined(NEED_sysdepCallMethod) */

#endif /* __parisc_sysdepCallMethod_h */

