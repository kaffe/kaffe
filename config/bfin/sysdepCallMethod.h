/*
 * arm/sysdepCallMethod.h
 * Common arm sysdepCallNethod.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * Written by Godmar Back <gback@cs.utah.edu>
 */

#ifndef __bfin_sysdepCallMethod_h
#define __bfin_sysdepCallMethod_h

#include "support.h"

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
#if defined(NEED_sysdepCallMethod)

static inline void sysdepCallMethod(callMethodInfo *call) {
	int allargs[call->nrargs];
	switch(call->nrargs) {
	register int r0 asm("R0");
	register int r1 asm("R1");
	register int r2 asm("R2");
	default:
	{
		int *args = allargs ;
		int argidx = 0;
		for(;argidx < (call)->nrargs ; ++argidx)
		{
			*args++ = (call)->args[argidx].i;
		}
	}
	case 3:
    		r2 = call->args[2].i;
  	case 2:
    		r1 = call->args[1].i;
  	case 1:
    		r0 = call->args[0].i;
  	case 0:
		asm volatile ("call      (%2)\n"
		         : "=r" (r0), "=r" (r1)
         		: "a" (call->function),
                    		"0" (r0), "1" (r1), "r" (r2)
         		);
		switch (call->rettype)
    		{
    			case 'V':
      				break;

    			case 'D':
				(&call->ret->i)[1] = r1;
			      (&call->ret->i)[0] = r0;
      				break;

    			case 'F':
      				call->ret->i = r0;
     	 			break;
 
    			case 'J':
      				(&call->ret->i)[1] = r1;
      				(&call->ret->i)[0] = r0;
      				break;

    			default:
      				call->ret->i = r0;
      				break;
    		}
	}
 
}

#endif /* defined(NEED_sysdepCallMethod) */

#endif /* __arm_sysdepCallMethod_h */
