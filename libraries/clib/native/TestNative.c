/*
 * TestNative.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "access.h"
#include "constants.h"
#include "object.h"
#include "classMethod.h"
#include "itypes.h"
#include "support.h"
#include "baseClasses.h"
#include <native.h>
#include "defs.h"

#if defined(KAFFE_VMDEBUG)
jint
TestNative_test16int(jint a, jint b, jint c, jint d, 
	jint e, jint f, jint g, jint h, 
	jint i, jint j, jint k, jint l, 
	jint m, jint n, jint o, jint p)
{
#define A(x)	printf("%s = %08x\n", #x, x), fflush(stdout)
	A(a); A(b); A(c); A(d);
	A(e); A(f); A(g); A(h);
	A(i); A(j); A(k); A(l);
	A(m); A(n); A(o); A(p);
	return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
}

jlong
TestNative_test16long(jlong a, jlong b, jlong c, jlong d, 
	jlong e, jlong f, jlong g, jlong h, 
	jlong i, jlong j, jlong k, jlong l, 
	jlong m, jlong n, jlong o, jlong p)
{
#define B(x)	printf("%s = %08x%08x\n", #x, (int)(x >> 32), (int)x), \
	fflush(stdout)
	B(a); B(b); B(c); B(d);
	B(e); B(f); B(g); B(h);
	B(i); B(j); B(k); B(l);
	B(m); B(n); B(o); B(p);
	return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
}

jlong
TestNative_test16intlong(jint a, jlong b, jlong c, jlong d, 
	jlong e, jlong f, jlong g, jlong h, 
	jlong i, jint j, jint k, jlong l, 
	jlong m, jlong n, jlong o, jlong p)
{
	A(a); B(b); B(c); B(d);
	B(e); B(f); B(g); B(h);
	B(i); A(j); A(k); B(l);
	B(m); B(n); B(o); B(p);
	return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
}

jfloat
TestNative_test16float(jfloat a, jfloat b, jfloat c, jfloat d, 
	jfloat e, jfloat f, jfloat g, jfloat h, 
	jfloat i, jfloat j, jfloat k, jfloat l, 
	jfloat m, jfloat n, jfloat o, jfloat p)
{
#define C(x)	printf("%s = %4.2f\n", #x, x), fflush(stdout)
	C(a); C(b); C(c); C(d);
	C(e); C(f); C(g); C(h);
	C(i); C(j); C(k); C(l);
	C(m); C(n); C(o); C(p);
	return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
}

jdouble
TestNative_test16double(jdouble a, jdouble b, jdouble c, jdouble d, 
	jdouble e, jdouble f, jdouble g, jdouble h, 
	jdouble i, jdouble j, jdouble k, jdouble l, 
	jdouble m, jdouble n, jdouble o, jdouble p)
{
	C(a); C(b); C(c); C(d);
	C(e); C(f); C(g); C(h);
	C(i); C(j); C(k); C(l);
	C(m); C(n); C(o); C(p);
	return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
}

jdouble
TestNative_test16floatdouble(jfloat a, jdouble b, jdouble c, jdouble d, 
	jdouble e, jdouble f, jdouble g, jdouble h, 
	jdouble i, jfloat j, jfloat k, jdouble l, 
	jdouble m, jdouble n, jdouble o, jdouble p)
{
	C(a); C(b); C(c); C(d);
	C(e); C(f); C(g); C(h);
	C(i); C(j); C(k); C(l);
	C(m); C(n); C(o); C(p);
	return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
}

#endif /* KAFFE_VMDEBUG */
