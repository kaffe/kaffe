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
#include "../../../kaffe/kaffevm/fp.h"
#include "defs.h"
#include "files.h"
#include "java_lang_Double.h"
#include <native.h>

/*
 * Convert a "normal" double to a string.
 *
 * Here "val" is the double or float value and "maxPrecision" is
 * the maximum number of digits after the decimal point that are
 * significant (and therefore should be displayed).
 *
 * This assumes printf(3) conforms to ISO 9899: 1990 (``ISO C'').
 */
struct Hjava_lang_String*
java_lang_Double_normalToString(jdouble val, jint maxPrecision)
{
	static const jlong negBit = 0x8000000000000000LL; /* "LL" gcc-ism */
	const jlong bits = java_lang_Double_doubleToLongBits(val);
	char *s, *t, buf[MAXNUMLEN];
	int k;

	/* Deal with negative numbers manually so negative zero is "-0.0" */
	s = buf;
	if (bits & negBit) {
		val = -val;
		*s++ = '-';
	}

	if (val == 0.0 || (val >= 1.0e-3 && val < 1.0e7)) {

		/* Print in decimal notation */
		sprintf(s, "%.*f", (int) maxPrecision, (double) val);

		/* Trim off trailing zeroes after the decimal point */
		for (k = strlen(buf) - 1;
		    buf[k] == '0' && buf[k - 1] != '.';
		    k--) {
			buf[k] = '\0';
		}
	} else {

		/* Print in exponential notation */
		sprintf(s, "%.*E", (int) maxPrecision, (double) val);

		/* Trim off the leading zero in the exponent, if any */
		s = strchr(buf, 'E');
		assert(s != NULL && (s[1] == '-' || s[1] == '+'));
		if (s[2] == '0' && s[3] != '\0') {
			memmove(s + 2, s + 3, strlen(s + 2));
		}

		/* Trim off trailing zeroes after the decimal point */
		for (t = s - 1; *t == '0' && t[-1] != '.'; t--);
		memmove(t + 1, s, strlen(s) + 1);
	}
	return (stringC2Java(buf));
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
	return(doubleToLong(val));
}

/*
 * Convert jlong to double.
 */
jdouble
java_lang_Double_longBitsToDouble(jlong val)
{
	return(longToDouble(val));
}

