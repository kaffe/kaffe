/*
 * fp.c
 *
 * Floating point routines
 *
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-hacks.h"
#include <math.h>
#include "gtypes.h"
#include "defs.h"
#include "files.h"
#include "fp.h"

/*
 * Bit patterns according to IEEE 754. Note use of "LL" gcc-ism.
 */

#define DEXPMASK	0x7ff0000000000000LL
#define DMANMASK	0x000fffffffffffffLL
#define DSIGNBIT	0x8000000000000000LL
#define DINFBITS	0x7ff0000000000000LL
#define DNANBITS	0x7ff8000000000000LL

#define FEXPMASK	0x7f800000
#define FMANMASK	0x007fffff
#define FSIGNBIT	0x80000000
#define FINFBITS	0x7f800000
#define FNANBITS	0x7fc00000

#define DISNAN(db)	((db & DEXPMASK) == DEXPMASK && (db & DMANMASK) != 0)
#define FISNAN(db)	((db & FEXPMASK) == FEXPMASK && (db & FMANMASK) != 0)

/*
 * Convert double to jlong.
 */
jlong
doubleToLong(jdouble val)
{
	jvalue d;
	d.d = val;

#if defined(DOUBLE_ORDER_OPPOSITE)
	{
		/* swap low and high word */
		uint32 r = *(uint32*)&d.j;
		uint32 *s = (uint32*)&d.j + 1;
		d.i = *s;
		*s = r;
	}
#endif
	return d.j;
}

/*
 * Convert jlong to double.
 */
jdouble
longToDouble(jlong val)
{
	jvalue d;

	/* Force all possible NaN values into the canonical NaN value */
	if ((val & DEXPMASK) == DEXPMASK && (val & DMANMASK) != 0)
		val = DNANBITS;

	/* Convert value */
	d.j = val;
#if defined(DOUBLE_ORDER_OPPOSITE)
	{
		/* swap low and high word */
		uint32 r = *(uint32*)&d.j;
		uint32 *s = (uint32*)&d.j + 1;
		d.i = *s;
		*s = r;
	}
#endif
	return d.d;
}

/*
 * Convert float to int.
 */
jint
floatToInt(jfloat val)
{
        jvalue d;

	d.f = val;
	return d.i;
}

/*
 * Convert int to float.
 */
float
intToFloat(jint val)
{
	jvalue d;

	/* Force all possible NaN values into the canonical NaN value */
	if ((val & FEXPMASK) == FEXPMASK && (val & FMANMASK) != 0)
		val = FNANBITS;

	/* Convert value */
	d.i = val;
	return d.f;
}

/*
 * Divide doubles accoring to Java rules
 */
jdouble
doubleDivide(jdouble v1, jdouble v2)
{
	jlong v1bits, v2bits;

	if (v2 != 0.0)
		return (v1 / v2);
	if (v1 == 0.0)
		return longToDouble(DNANBITS);
	v1bits = doubleToLong(v1);
	if (DISNAN(v1bits))
		return longToDouble(DNANBITS);
	v2bits = doubleToLong(v2);
	return longToDouble(DINFBITS | ((v1bits ^ v2bits) & DSIGNBIT));
}

/*
 * Divide floats accoring to Java rules
 */
jfloat
floatDivide(jfloat v1, jfloat v2)
{
	jint v1bits, v2bits;

	if (v2 != 0.0)
		return (v1 / v2);
	if (v1 == 0.0)
		return intToFloat(FNANBITS);
	v1bits = floatToInt(v1);
	if (FISNAN(v1bits))
		return intToFloat(FNANBITS);
	v2bits = floatToInt(v2);
	return intToFloat(FINFBITS | ((v1bits ^ v2bits) & FSIGNBIT));
}

