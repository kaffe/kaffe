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
#include "baseClasses.h"
#include "gtypes.h"
#include "defs.h"
#include "files.h"
#include "fp.h"

/*
 * Convert float to int.
 */
static
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
static
float
intToFloat(jint val)
{
	jvalue d;

	/* Convert value */
	d.i = val;

	/* Force all possible NaN values into the canonical NaN value */
	if (isnan(d.f))
	  return KAFFE_JFLOAT_NAN;

	return d.f;
}

/*
 * Add doubles accoring to Java rules
 */
jdouble
doubleAdd(jdouble v1, jdouble v2)
{
	if (isnan(v1) || isnan(v2)) {
		return KAFFE_JDOUBLE_NAN;
	}
	return (v1 + v2);
}

/*
 * Add floats accoring to Java rules
 */
jfloat
floatAdd(jfloat v1, jfloat v2)
{
	if (isnan(v1) || isnan(v2)) {
		return KAFFE_JFLOAT_NAN;
	}
	return (v1 + v2);
}

/*
 * Substract doubles accoring to Java rules
 */
jdouble
doubleSubtract(jdouble v1, jdouble v2)
{
	if (isnan(v1) || isnan(v2)) {
	  return KAFFE_JDOUBLE_NAN;
	}
	return (v1 - v2);
}

/*
 * Subtract floats accoring to Java rules
 */
jfloat
floatSubtract(jfloat v1, jfloat v2)
{
	if (isnan(v1) || isnan(v2)) {
		return KAFFE_JFLOAT_NAN;
	}
	return (v1 - v2);
}

/*
 * Multiply doubles accoring to Java rules
 */
jdouble
doubleMultiply(jdouble v1, jdouble v2)
{
	if (isnan(v1) || isnan(v2)) {
	  return KAFFE_JDOUBLE_NAN;
	}
	return (v1 * v2);
}

/*
 * Multiply floats accoring to Java rules
 */
jfloat
floatMultiply(jfloat v1, jfloat v2)
{
	if (isnan(v1) || isnan(v2)) {
		return KAFFE_JFLOAT_NAN;
	}
	return (v1 * v2);
}

/*
 * Divide doubles accoring to Java rules
 */
jdouble
doubleDivide(jdouble v1, jdouble v2)
{
	jboolean negative_result;

	if (isnan(v1) || isnan(v2)) 
		return KAFFE_JDOUBLE_NAN;

	negative_result = ((signbit(v1) == 0) && (signbit(v2) != 0))
			    || ((signbit(v1) != 0) && (signbit(v2) == 0));

	if (isinf(v1) && isinf(v2)) 
		return KAFFE_JDOUBLE_NAN;

	if (isinf(v1) && isfinite(v2)) {
		if (negative_result)
			return KAFFE_JDOUBLE_NEG_INF;
		else
			return KAFFE_JDOUBLE_POS_INF;
	}

	if (isfinite(v1) && isinf(v2)) {
		if (negative_result)
			return -0.0;
		else
			return 0.0;
	}

	if (v1 == 0.0) {
		if (v2 == 0.0)
		        return KAFFE_JDOUBLE_NAN;
		else if (negative_result)
			return -0.0;
		else
			return 0.0;
	}

	if (v1 != 0.0 && v2 == 0.0) {
		if (negative_result)
			return KAFFE_JDOUBLE_NEG_INF;
		else
			return KAFFE_JDOUBLE_POS_INF;
	}

	return (v1 / v2);
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

	if (isnan(v1) || isnan(v2)) {
		return KAFFE_JFLOAT_NAN;
	}
	if (v2 != 0.0) {
		return (v1 / v2);
	}
	if (v1 == 0.0) {
		return KAFFE_JFLOAT_NAN;
	}
	return intToFloat((jint)(FINFBITS | ((v1bits ^ v2bits) & FSIGNBIT)));
}
