/*
 * mips/common.h
 * Common MIPS configuration information.
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
 * by Alexandre Oliva <oliva@dcc.unicamp.br>
 * and Edouard G. Parmelan <egp@free.fr>
 */

#ifndef __mips_common_h
#define __mips_common_h

/* The R5900 is the Mips Core in the PS2 (Playstation 2)
* It has most of the Mips III instructions set, some of the Mips IV 
* instructions, it lacks the Mips II "ll" and "sc" instructions and 
* it has 128 bit GP registers and 32 bit FPU registers. There is no 
* FPU Emulation present in the default kernel. Since no 64 bit
* FPU registers exist, doubles are passed in 2 GP registers.
*/
#ifdef _R5900
#define PS2LINUX
#undef HAVE_MIPSII_INSTRUCTIONS
#endif 

#if defined(HAVE_MIPSII_INSTRUCTIONS)
#include "atomicity.h"
#endif /* defined(HAVE_MIPSII_INSTRUCTIONS) */

#if defined(HAVE_MIPSII_INSTRUCTIONS)

/*
 * Do an atomic compare and exchange.  The address 'A' is checked against
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is sucessful, otherwise 0.
 */
#define COMPARE_AND_EXCHANGE(A,O,N)	(compare_and_swap((long int*) A, (long int) O, (long int) N))

#else

/*
 * Do an atomic compare and exchange.  The address 'A' is checked against
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is sucessful, otherwise 0.
 */
#define COMPARE_AND_EXCHANGE(A,O,N)		\
({						\
    int ret = 0;				\
    KTHREAD(suspendall)();			\
						\
    if (*(A) == (O)) {				\
	*(A) = (N);				\
	ret = 1;				\
    }						\
    KTHREAD(unsuspendall)();			\
    ret;					\
})

#endif
    
#endif
