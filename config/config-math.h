/*
 * config-math.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __config_math_h
#define __config_math_h

#include <math.h>
#include <limits.h>

/*
 * Java defines two sorts of floating-point remainder operations, one
 * IEEE 754 remainder (for java.lang.IEEEremainder()) and a
 * different version for the % operator.
 */

/* IEEE 754 remainder for doubles */
#if defined(HAVE_REMAINDER)
#define IEEERemainder(a, b) remainder(a, b)
#elif defined(HAVE_DREM)
#define IEEERemainder(a, b) drem(a, b)
#else
#define	IEEERemainder(a, b) 0
/* Kaffe requires a truncating-division-based floating-point remainder operation */
#endif

/* IEEE 754 remainder for floats */
#if defined(HAVE_REMAINDERF)
#define IEEERemainderf(a, b) remainderf(a, b)
#else
#define IEEERemainderf(a, b) ((float)IEEERemainder((double)a, (double)b))
#endif

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

#if !defined(HAVE_RINT)
/* As per Java Class Libraries page 927 */
#define	rint(v)		(floor(v+0.5))
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
