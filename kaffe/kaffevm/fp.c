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
 *
 * If you change this function, also change strtod("-0.0") check in
 * configure.in.
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
 * Add doubles accoring to Java rules
 */
jdouble
doubleAdd(jdouble v1, jdouble v2)
{
	jlong v1bits, v2bits;

	v1bits = doubleToLong(v1);
	v2bits = doubleToLong(v2);

	if (DISNAN(v1bits) || DISNAN(v2bits)) {
		return longToDouble(DNANBITS);
	}
	return (v1 + v2);
}

/*
 * Add floats accoring to Java rules
 */
jfloat
floatAdd(jfloat v1, jfloat v2)
{
	jint v1bits, v2bits;

	v1bits = floatToInt(v1);
	v2bits = floatToInt(v2);

	if (FISNAN(v1bits) || FISNAN(v2bits)) {
		return intToFloat(FNANBITS);
	}
	return (v1 + v2);
}

/*
 * Substract doubles accoring to Java rules
 */
jdouble
doubleSubtract(jdouble v1, jdouble v2)
{
	jlong v1bits, v2bits;

	v1bits = doubleToLong(v1);
	v2bits = doubleToLong(v2);

	if (DISNAN(v1bits) || DISNAN(v2bits)) {
		return longToDouble(DNANBITS);
	}
	return (v1 - v2);
}

/*
 * Subtract floats accoring to Java rules
 */
jfloat
floatSubtract(jfloat v1, jfloat v2)
{
	jint v1bits, v2bits;

	v1bits = floatToInt(v1);
	v2bits = floatToInt(v2);

	if (FISNAN(v1bits) || FISNAN(v2bits)) {
		return intToFloat(FNANBITS);
	}
	return (v1 - v2);
}

/*
 * Multiply doubles accoring to Java rules
 */
jdouble
doubleMultiply(jdouble v1, jdouble v2)
{
	jlong v1bits, v2bits;

	v1bits = doubleToLong(v1);
	v2bits = doubleToLong(v2);

	if (DISNAN(v1bits) || DISNAN(v2bits)) {
		return longToDouble(DNANBITS);
	}
	return (v1 * v2);
}

/*
 * Multiply floats accoring to Java rules
 */
jfloat
floatMultiply(jfloat v1, jfloat v2)
{
	jint v1bits, v2bits;

	v1bits = floatToInt(v1);
	v2bits = floatToInt(v2);

	if (FISNAN(v1bits) || FISNAN(v2bits)) {
		return intToFloat(FNANBITS);
	}
	return (v1 * v2);
}

/*
 * Divide doubles accoring to Java rules
 */
jdouble
doubleDivide(jdouble v1, jdouble v2)
{
	jlong v1bits, v2bits;

	v1bits = doubleToLong(v1);
	v2bits = doubleToLong(v2);

	if (DISNAN(v1bits) || DISNAN(v2bits)) {
		return longToDouble(DNANBITS);
	}
	if (v2 != 0.0) {
		return (v1 / v2);
	}
	if (v1 == 0.0) {
		return longToDouble(DNANBITS);
	}
	return longToDouble((jlong)(DINFBITS | ((v1bits ^ v2bits) & DSIGNBIT)));
}

/*
 * Divide floats accoring to Java rules
 */
jfloat
floatDivide(jfloat v1, jfloat v2)
{
	jint v1bits, v2bits;

	v1bits = floatToInt(v1);
	v2bits = floatToInt(v2);

	if (FISNAN(v1bits) || FISNAN(v2bits)) {
		return intToFloat(FNANBITS);
	}
	if (v2 != 0.0) {
		return (v1 / v2);
	}
	if (v1 == 0.0) {
		return intToFloat(FNANBITS);
	}
	return intToFloat((jint)(FINFBITS | ((v1bits ^ v2bits) & FSIGNBIT)));
}
