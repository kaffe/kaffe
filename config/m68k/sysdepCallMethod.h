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

#ifndef __m68k_sysdepCallMethod_h
#define __m68k_sysdepCallMethod_h

#include "support.h"

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
static inline void sysdepCallMethod(callMethodInfo *call) ALWAYS_INLINE;

static inline void sysdepCallMethod(callMethodInfo *call) 
{
	int extraargs[(call)->nrargs];
	register int d0 asm ("d0");
	register int d1 asm ("d1");
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

#endif /* __m68k_sysdepCallMethod_h */
