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

#if defined(HAVE_REMAINDER)
#elif defined(HAVE_FMOD)
#define	remainder	fmod
#elif defined(HAVE_DREM)
#define	remainder	drem
#else
#error "Need some form of remainder"
#endif
#if !defined(HAVE_REMAINDERF)
#define	remainderf(a, b) (float)remainder((double)a, (double)b)
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
