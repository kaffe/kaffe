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
	asm volatile("swap [%2],%0" : "=r" (N) : "0" (N), "r" (A) : "cc", "memory" );

#endif
