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

#define atomic_compare_and_exchange_val_acq(A, N, O) (compare_and_swap((long int *) A, (long int) O, (long int) N))

#include "katomic.h"
#include "generic/comparexch.h"

#endif
