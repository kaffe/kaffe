/*
 * java.lang.Double.c
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
#include "config-hacks.h"
#include <math.h>
#include "../../../kaffe/kaffevm/classMethod.h"
#include "../../../kaffe/kaffevm/gtypes.h"
#include "../../../kaffe/kaffevm/locks.h"
#include "../../../kaffe/kaffevm/stringSupport.h"
#include "defs.h"
#include "files.h"
#include "Double.h"
#include <native.h>

/*
 * Convert double to string.
 */
struct Hjava_lang_String*
java_lang_Double_toString(jdouble val)
{
	char str[MAXNUMLEN];

	sprintf(str, "%.11g", val);
	return (stringC2Java(str));
}

/*
 * Convert string to double object.
 */
double
java_lang_Double_valueOf0(struct Hjava_lang_String* str)
{
	double value;
	char buf[MAXNUMLEN];
	char* endbuf;
	char* msg = "Bad float/double format";

	/* stringJava2CBuf would silently promote 0 to an empty string */
	if (str == 0) {
		SignalError("java.lang.NullPointerException", "");
	}

	stringJava2CBuf(str, buf, sizeof(buf));

#if defined(HAVE_STRTOD)
	value = strtod(buf, &endbuf);
	while (*endbuf != 0) {
		switch (*endbuf) {
		case ' ':
		case '\t':
		case '\n':
		case '\r':	/* Ignore whitespace */
		case 'F':
		case 'f':	/* Ignore float suffix. */
		case 'D':
		case 'd':	/* Ignore double suffix. */
			endbuf++;
			break;
		default:
			goto bail;
		}
	}
	/* don't return 0 if string was empty */
	if (endbuf == buf) {
		msg = "empty string";
		goto bail;
	}
	if (errno == ERANGE) {
		if (value == HUGE_VAL || value == -HUGE_VAL) {
			msg = "Overflow";
			goto bail;
		}
		if (value == 0.0) {
			msg = "Underflow";
			goto bail;
		}
	} 
#else
	/* Fall back on old atof - no error checking */
	value = atof(buf);
#endif

	return (value);

bail:;
	SignalError("java.lang.NumberFormatException", msg);
	return (0);
}

/*
 * Convert string into double class. (JDK 1.0.2)
 */
struct Hjava_lang_Double*
java_lang_Double_valueOf(struct Hjava_lang_String* str)
{
	struct Hjava_lang_Double* obj;
	obj = (struct Hjava_lang_Double*)execute_java_constructor("java.lang.Double", 0, "(D)V", java_lang_Double_valueOf0(str));
	return (obj);
}

/*
 * Convert double to jlong.
 */
jlong
java_lang_Double_doubleToLongBits(jdouble val)
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
java_lang_Double_longBitsToDouble(jlong val)
{
	jvalue d;
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
