/*
 * sparc/common.h
 * Common SPARC configuration information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * by Alexandre Oliva <oliva@dcc.unicamp.br>
 */

#ifndef __sparc_common_h
#define __sparc_common_h

#if NEED_sysdepCallMethod

#define LONG_SYSDEP 1
#include "sysdepCallMethod.h"
#undef LONG_SYSDEP
#include "sysdepCallMethod.h"

#endif /* NEED_sysdepCallMethod */


/*
 * Do an atomic exchange.  The contents of address 'A' is exchanged
 * with value 'N'.
 */
#define ATOMIC_EXCHANGE(A,N) \
	asm volatile("swap [%2],%0" : "=&r" (N) : "0" (N), "r" (A) : "cc", "memory" );

#if 0
#undef ATOMIC_EXCHANGE
#warning "jthread COMPARE_AND_EXCHANGE"

/*
 * Do an atomic compare and exchange.  The address 'A' is checked against
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is sucessful, otherwise 0.
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

#endif
