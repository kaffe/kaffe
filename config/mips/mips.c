/*
 * mips/mips.c
 * MIPS specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "jtypes.h"
#include "object.h"
#include "support.h"

#if _MIPS_SIM == _MIPS_SIM_NABI32
void
mipsN32CallMethod (callMethodInfo *CALL)
{
  typedef long long regword;
  int i, nrargs;
  for (i = 0, nrargs = 0; i < (CALL)->nrargs; ++i) {
    if ((CALL)->callsize[i] != 0) {
      if (i > nrargs) {
	(CALL)->args[nrargs] = (CALL)->args[i];
	(CALL)->callsize[nrargs] = (CALL)->callsize[i];
	(CALL)->calltype[nrargs] = (CALL)->calltype[i];
      }
      ++nrargs;
    }
  }
  {
    register int	iret	__asm__("$2");
    register regword	jret	__asm__("$2");
    register float	fret	__asm__("$f0");
    register double	dret	__asm__("$f0");
    register int	i0	__asm__("$4");
    register regword	j0	__asm__("$4");
    register int	i1	__asm__("$5");
    register regword	j1	__asm__("$5");
    register int	i2	__asm__("$6");
    register regword	j2	__asm__("$6");
    register int	i3	__asm__("$7");
    register regword	j3	__asm__("$7");
    register int	i4	__asm__("$8");
    register regword	j4	__asm__("$8");
    register int	i5	__asm__("$9");
    register regword	j5	__asm__("$9");
    register int	i6	__asm__("$10");
    register regword	j6	__asm__("$10");
    register int	i7	__asm__("$11");
    register regword	j7	__asm__("$11");
    register regword*	sp	__asm__("sp");
    register regword*	gp	__asm__("gp");
    register regword*	ra	__asm__("ra");
    register regword	(*t9)()	__asm__("$25");
    regword* curr_sp = sp;
    regword* curr_gp = gp;
    regword* curr_ra = ra;

    switch (nrargs) {
    default:
    {
      regword *argspace = __builtin_alloca((nrargs-8)*sizeof (regword));
      regword *argframe = (regword *)sp;
      regword *args = argframe;
      int argidx = 8;
      for (; argidx < nrargs; ++argidx, ++args) {
	if ((CALL)->calltype[argidx] == 'F')
	  *(float*)args = (CALL)->args[argidx].f;
	else if ((CALL)->calltype[argidx] == 'D')
	  *(double*)args = (CALL)->args[argidx].d;
	else if ((CALL)->callsize[argidx] == 2)
	  *args = (CALL)->args[argidx].j;
	else
	  *args = (CALL)->args[argidx].i;
      }
    }
    case 8:
      if ((CALL)->calltype[7] == 'D')
        asm volatile ("l.d $f19,%0" : : "m" ((CALL)->args[7].d));
      else if ((CALL)->calltype[7] == 'F')
        asm volatile ("l.s $f19,%0" : : "m" ((CALL)->args[7].f));
      if ((CALL)->callsize[7] == 2)
	j7 = (CALL)->args[7].j;
      else
	i7 = (CALL)->args[7].i;
    case 7:
      if ((CALL)->calltype[6] == 'D')
        asm volatile ("l.d $f18,%0" : : "m" ((CALL)->args[6].d));
      else if ((CALL)->calltype[6] == 'F')
        asm volatile ("l.s $f18,%0" : : "m" ((CALL)->args[6].f));
      if ((CALL)->callsize[6] == 2)
	j6 = (CALL)->args[6].j;
      else
	i6 = (CALL)->args[6].i;
    case 6:
      if ((CALL)->calltype[5] == 'D')
        asm volatile ("l.d $f17,%0" : : "m" ((CALL)->args[5].d));
      else if ((CALL)->calltype[5] == 'F')
        asm volatile ("l.s $f17,%0" : : "m" ((CALL)->args[5].f));
      if ((CALL)->callsize[5] == 2)
	j5 = (CALL)->args[5].j;
      else
	i5 = (CALL)->args[5].i;
    case 5:
      if ((CALL)->calltype[4] == 'D')
        asm volatile ("l.d $f16,%0" : : "m" ((CALL)->args[4].d));
      else if ((CALL)->calltype[4] == 'F')
        asm volatile ("l.s $f16,%0" : : "m" ((CALL)->args[4].f));
      if ((CALL)->callsize[4] == 2)
	j4 = (CALL)->args[4].j;
      else
	i4 = (CALL)->args[4].i;
    case 4:
      if ((CALL)->calltype[3] == 'D')
        asm volatile ("l.d $f15,%0" : : "m" ((CALL)->args[3].d));
      else if ((CALL)->calltype[3] == 'F')
        asm volatile ("l.s $f15,%0" : : "m" ((CALL)->args[3].f));
      if ((CALL)->callsize[3] == 2)
	j3 = (CALL)->args[3].j;
      else
	i3 = (CALL)->args[3].i;
    case 3:
      if ((CALL)->calltype[2] == 'D')
        asm volatile ("l.d $f14,%0" : : "m" ((CALL)->args[2].d));
      else if ((CALL)->calltype[2] == 'F')
        asm volatile ("l.s $f14,%0" : : "m" ((CALL)->args[2].f));
      if ((CALL)->callsize[2] == 2)
	j2 = (CALL)->args[2].j;
      else
	i2 = (CALL)->args[2].i;
    case 2:
      if ((CALL)->calltype[1] == 'D')
        asm volatile ("l.d $f13,%0" : : "m" ((CALL)->args[1].d));
      else if ((CALL)->calltype[1] == 'F')
        asm volatile ("l.s $f13,%0" : : "m" ((CALL)->args[1].f));
      if ((CALL)->callsize[1] == 2)
	j1 = (CALL)->args[1].j;
      else
	i1 = (CALL)->args[1].i;
    case 1:
      if ((CALL)->calltype[0] == 'D')
        asm volatile ("l.d $f12,%0" : : "m" ((CALL)->args[0].d));
      else if ((CALL)->calltype[0] == 'F')
        asm volatile ("l.s $f12,%0" : : "m" ((CALL)->args[0].f));
      if ((CALL)->callsize[0] == 2)
	j0 = (CALL)->args[0].j;
      else
	i0 = (CALL)->args[0].i;
    case 0:
      /* call function with 1st 8 args */
      asm ("" : :
	   "r" (i0), "r" (i1), "r" (i2), "r" (i3),
	   "r" (i4), "r" (i5), "r" (i6), "r" (i7));
      asm ("" : :
	   "r" (j0), "r" (j1), "r" (j2), "r" (j3),
	   "r" (j4), "r" (j5), "r" (j6), "r" (j7));

      (*(t9 = (CALL)->function))();

      asm ("lw $31,%0" : : "m" (curr_ra));
      asm ("lw $gp,%0" : : "m" (curr_gp));
      asm ("lw $sp,%0" : : "m" (curr_sp));

      if ((CALL)->retsize != 0) {
	switch((CALL)->retsize) {
	case 2:
	  if ((CALL)->rettype == 'D')
	    (CALL)->ret->d = dret;
	  else {
	    (CALL)->ret->j = jret;
	  }
	  break;
	case 1:
	  if ((CALL)->rettype == 'F')
	    (CALL)->ret->f = fret;
	  else
	    (CALL)->ret->i = iret;
	  break;
	default:
	  ABORT();
	  break;
	}
      }
      break;
    }
  }
}
#endif /* _MIPS_SIM == _MIPS_SIM_NABI32 */

#if _MIPS_SIM == _MIPS_SIM_ABI32
void
mips32CallMethod (callMethodInfo *CALL)
{
    register int	iret	__asm__("$2");
    register int64	jret	__asm__("$2");
    register float	fret	__asm__("$f0");
    register double	dret	__asm__("$f0");

    register int	i0	__asm__("$4");
    register int	i1	__asm__("$5");
    register int	i2	__asm__("$6");
    register int	i3	__asm__("$7");

    register int*	sp	__asm__("sp");
    register int*	gp	__asm__("gp");
    register int*	ra	__asm__("ra");
    register int64	(*t9)(void) __asm__("$25");
    int* curr_sp = sp;
    int* curr_gp = gp;
    int* curr_ra = ra;

    {
      int nrargs = (CALL)->nrargs;
      uintp *argframe = __builtin_alloca((nrargs > 4 ? nrargs : 4) * sizeof(int64));
      int argidx;
      int idx = 0;

      for (argidx = 0; argidx < nrargs; ++argidx) {
	switch ((CALL)->callsize[argidx]) {
	case 2:
	  if ((idx & 1) != 0) {	/* Align */
	    idx += 1;
	  }
	  *(jlong*)&argframe[idx] = (CALL)->args[argidx].j;
	  idx += 2;
	  break;
	case 1:
	  *(jint*)&argframe[idx] = (CALL)->args[argidx].i;
	  idx += 1;
	  break;
	default:
	  break;
	}
      }

      i0 = argframe[0];
      i1 = argframe[1];
      i2 = argframe[2];
      i3 = argframe[3];

      sp = argframe;

      asm ("" : : "r" (i0), "r" (i1), "r" (i2), "r" (i3));

      (*(t9 = (CALL)->function))();

      asm ("lw $31,%0" : : "m" (curr_ra));
      asm ("lw $gp,%0" : : "m" (curr_gp));
      asm ("lw $sp,%0" : : "m" (curr_sp));

      if ((CALL)->retsize != 0) {
	switch((CALL)->retsize) {
	case 2:
	  if ((CALL)->rettype == 'D')
	    (CALL)->ret->d = dret;
	  else {
	    (CALL)->ret->j = jret;
	  }
	  break;
	case 1:
	  if ((CALL)->rettype == 'F')
	    (CALL)->ret->f = fret;
	  else
	    (CALL)->ret->i = iret;
	  break;
	default:
	  ABORT();
	  break;
	}
      }
    }
}
#endif /* _MIPS_SIM == _MIPS_SIM_ABI32 */
