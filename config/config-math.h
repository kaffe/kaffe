/*
 * config-math.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"

#ifndef __config_math_h
#define __config_math_h

#include <math.h>
#include <limits.h>

/* Java remainder for doubles */
#if defined(HAVE_FMOD)
#define javaRemainder(a, b) fmod(a, b)
#else
#define	javaRemainder(a, b) 0
/* Kaffe requires a truncating-division-based floating-point remainder operation */
#endif

/* Java remainder for floats */
#if defined(HAVE_FMODF)
#define javaRemainderf(a, b) fmodf(a, b)
#else
#define javaRemainderf(a, b) ((float)javaRemainder((double)a, (double)b))
#endif

#if !defined(HAVE_FLOOR)
#define	floor(v)	((double)(long)(v))
#endif

#if !defined(HAVE_CEIL)
#define	ceil(v)		(-floor(-(v)))
#endif

/* If we dont' have isinf() assume nothing is */
#if !defined(HAVE_ISINF)
#define	isinf(x)	0
#endif

/* If we don't have isnan() assume nothing is */
#if !defined(HAVE_ISNAN)
#define	isnan(x)	0
#endif

#endif
