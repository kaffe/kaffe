/*
 * arm/common.h
 * Common arm configuration information.
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

#ifndef __arm_common_h
#define __arm_common_h

#include "atomicity.h"

/* The arm never aligns to more than a 4 byte boundary. */
#define	ALIGNMENT_OF_SIZE(S)	((S) < 4 ? (S) : 4)

/*
 * Do an atomic compare and exchange.  The address 'A' is checked against
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is successful, otherwise 0.
 */


#define COMPARE_AND_EXCHANGE(A, O, N)  (compare_and_swap((long int*) A, (long int) O, (long int) N))

#endif
