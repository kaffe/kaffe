
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

/*
 * Bit patterns according to IEEE 754.
 */

#define DEXPMASK	(((uint64)0x7ff00000)<<32)
#define DMANMASK	((((uint64)0x000fffff)<<32)|((uint64)0xffffffff))
#define DSIGNBIT	(((uint64)0x80000000)<<32)
#define DINFBITS	(((uint64)0x7ff00000)<<32)
#define DNANBITS	(((uint64)0x7ff80000)<<32)

#define FEXPMASK	0x7f800000
#define FMANMASK	0x007fffff
#define FSIGNBIT	0x80000000
#define FINFBITS	0x7f800000
#define FNANBITS	0x7fc00000

#define DISNAN(b)	(((b) & DEXPMASK) == DEXPMASK && ((b) & DMANMASK) != 0)
#define FISNAN(b)	(((b) & FEXPMASK) == FEXPMASK && ((b) & FMANMASK) != 0)

extern jfloat	floatAdd(jfloat v1, jfloat v2);
extern jdouble	doubleAdd(jdouble v1, jdouble v2);
extern jfloat	floatSubtract(jfloat v1, jfloat v2);
extern jdouble	doubleSubtract(jdouble v1, jdouble v2);
extern jfloat	floatMultiply(jfloat v1, jfloat v2);
extern jdouble	doubleMultiply(jdouble v1, jdouble v2);
extern jfloat	floatDivide(jfloat v1, jfloat v2);
extern jdouble	doubleDivide(jdouble v1, jdouble v2);

extern jlong	doubleToLong(jdouble val);
extern jdouble	longToDouble(jlong val);
extern jint	floatToInt(jfloat val);
extern float	intToFloat(jint val);

#endif /* __fp_h */

