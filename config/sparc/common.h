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

/* Include the right compare_and_swap definition */
#if defined(__sparcv9)
#include "atomicity-sparc32v9.h"
#else /* !defined(__sparcv9) */
#if defined(__arch64__)
#include "atomicity-sparc64.h"
#else /* !defined(__arch64__) */
#include "atomicity-sparc32.h"
#endif /* defined(__arch64__) */
#endif /* defined(__sparcv9) */

#if NEED_sysdepCallMethod

#define LONG_SYSDEP 1
#include "sysdepCallMethod.h"
#undef LONG_SYSDEP
#include "sysdepCallMethod.h"

#endif /* NEED_sysdepCallMethod */

/*
 * Do an atomic compare and exchange.  The address 'A' is checked against
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is successful, otherwise 0.
 */

#define COMPARE_AND_EXCHANGE(A, O, N)  (compare_and_swap((long int*) A, (long int) O, (long int) N))

#endif
