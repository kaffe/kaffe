/*
 * m68k/common.h
 * Common M68K configuration information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * Written by Kiyo Inaba <k-inaba@mxb.mesh.ne.jp>, 1998;
 * Based on the ports
 *      by Remi Perrot <r_perrot@mail.club-internet.fr> to m68k/linux
 * and
 *      by Alexandre Oliva <oliva@dcc.unicamp.br> to sparc
 *
 * Modified by Kiyo Inaba <k-inaba@mxb.mesh.ne.jp>, 1999 to support
 *	both linux and netbsd1.
 */

#ifndef __m68k_common_h
#define __m68k_common_h

#include "atomicity.h"

#include "support.h"

/*
 * Alignment in structure is 2 bytes packed.
 */
#define ALIGNMENT_OF_SIZE(S)    (((S>1)?2:1))

/*
 * sysdepCallMethod supports:
 *
 *	Calling sequences for linux and netbsd1 are same, except for
 *	the place of return values. The float/double values are in
 *	fp0 (linux) or d0/d1 (netbsd1).
 *
 *	Still I do not understand 'asm' statement well, and the
 *	statement below is a 'because it works' version.
 */
#if defined(__linux__)		// try forcing linux sysdepCallMethod()
#ifdef	USE_ASM		/* Old and obsolete... */
static inline void sysdepCallMethod(callMethodInfo *call)
{

	asm volatile (" \n"
		      "       1: \n"
		      "               cmp.l   #0,%0 \n"
		      "               beq     3f \n"
		      "               subq.l  #1,%0 \n"
		      "               cmp.b   #0,(%2,%0.l) \n"
		      "               beq     1b \n"
		      "               cmp.b   #1,(%2,%0.l) \n"
		      "               beq     2f \n"
		      "               move.l  4(%1,%0.l*8),-(%/sp) \n"
		      "       2: \n"
		      "               move.l  (%1,%0.l*8),-(%/sp) \n"
		      "               jmp     1b \n"
		      "       3: \n"
		      "               jsr     (%3) \n"
		      "               cmp.b   #0x46,%4 \n"
		      "               bne     4f \n"
		      "               fmove.s %/fp0,(%5) \n"
		      "               jmp     6f \n"
		      "       4: \n"
		      "               cmp.b   #0x44,%4 \n"
		      "               bne     5f \n"
		      "               fmove.d %/fp0,(%5) \n"
		      "               jmp     6f \n"
		      "       5: \n"
		      "               move.l  %/d0,(%5) \n"
		      "               cmp.b   #0x4a,%4 \n"
		      "               bne     6f \n"
		      "               move.l  %/d1,4(%5) \n"
		      "       6: \n"
		      "       " :
		      : "r" ((call)->nrargs),
		      "a" ((call)->args),
		      "a" ((call)->callsize),
		      "a" ((call)->function),
		      "m" ((call)->rettype),
		      "a" ((call)->ret)
		      : "d0", "d1", "fp0", "cc", "memory");
	asm volatile (" \n"
		      "               add.w %0,%/sp \n"
		      "        " : 
		      : "r" ((call)->argsize * sizeof(jint)) 
		      : "cc");
}
		       
#else	/* undef USE_ASM */
//	Linux version
static inline void sysdepCallMethod(callMethodInfo *call)
{
	int extraargs[(call)->nrargs];
	register int d0 asm ("d0");
	register int d1 asm ("d1");
	register double f0d asm ("fp0");
	register float f0f asm ("fp0");
	int *res;
	int *args = extraargs;
	int argidx;
	for(argidx = 0; argidx < (call)->nrargs; ++argidx) {
		if ((call)->callsize[argidx])
			*args++ = (call)->args[argidx].i;
		else
			*args++ = (call)->args[argidx-1].j;
	}
	asm volatile ("jsr	%2@\n"
		      : "=r" (d0), "=r" (d1)
		      : "a" ((call)->function)
		      : "cc", "memory");
	if ((call)->retsize != 0) {
		res = (int *)(call)->ret;
	switch((call)->retsize) {
	case 2:
	  if ((call)->rettype == 'D')
	    *(double*)res = f0d;
	  else {
		res[1] = d1;
		res[0] = d0;
	  }
	  break;
	case 1:
	  if ((call)->rettype == 'F')
	    *(double*)res = f0f;
	  else
		res[0] = d0;
	  break;
	}
	}
}
#endif	/* USE_ASM */
#else	/* not defined(__linux__) */

static inline void sysdepCallMethod(callMethodInfo *call) 
{
	int extraargs[(call)->nrargs];
	register int d0 asm ("d0");
	register int d1 asm ("d1");
	register double f0d asm ("fp0");
	register float f0f asm ("fp0");
	int *res;
	int *args = extraargs;
	int argidx;
	for(argidx = 0; argidx < (call)->nrargs; ++argidx)
	{
		if ((call)->callsize[argidx])
			*args++ = (call)->args[argidx].i;
		else
			*args++ = (call)->args[argidx-1].j;
	}
	asm volatile ("jsr	%2@\n"
	 : "=r" (d0), "=r" (d1)
	 : "a" ((call)->function)
	 : "cc", "memory");
	if ((call)->retsize >= 1 )
	{
		res = (int *)(call)->ret;
		res[0] = d0;
  		if((call)->retsize > 1)	/* >= 2 */
			res[1] = d1;
	}
}

#endif	/* defined(__linux__) */

/*
 * Do an atomic compare and exchange.  The address 'A' is checked against
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is successful, otherwise 0.
 */


#define COMPARE_AND_EXCHANGE(A, O, N)  (compare_and_swap((long int*) A, (long int) O, (long int) N)) 

#endif
