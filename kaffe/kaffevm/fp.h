
/*
 * fp.h
 *
 * Floating point routines
 *
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __fp_h
#define __fp_h

extern jfloat	floatDivide(jfloat v1, jfloat v2);
extern jdouble	doubleDivide(jdouble v1, jdouble v2);

extern jlong	doubleToLong(jdouble val);
extern jdouble	longToDouble(jlong val);
extern jint	floatToInt(jfloat val);
extern float	intToFloat(jint val);

#endif /* __fp_h */

