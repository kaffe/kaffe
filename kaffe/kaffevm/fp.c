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

