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
#include <ctype.h>
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
 * Convert a "normal" double to a string with the supplied precision.
 *
 * This assumes printf(3) conforms to ISO 9899: 1990 (``ISO C'').
 *
 * XXX We still print some values incorrectly. For example the double
 * XXX 0x400B333333333333 should be displayed as "3.4" instead of what
 * XXX we display, "3.3999999999999999".
 */
struct Hjava_lang_String*
java_lang_Double_toStringWithPrecision(jdouble val, jint precision)
{
	const jlong bits = java_lang_Double_doubleToRawLongBits(val);
	char *s, buf[MAXNUMLEN];
	int k;

	/* Deal with negative numbers manually so negative zero is "-0.0" */
	s = buf;
	if (bits & DSIGNBIT) {
		val = -val;
		*s++ = '-';
	}

	/* Print in normal or 'scientific' form according to value */
	if (val == 0.0 || (val >= 1.0e-3 && val < 1.0e7)) {
		static const double powTen[] = {
		  1.0e0, 1.0e1, 1.0e2, 1.0e3, 1.0e4, 1.0e5, 1.0e6
		};

		/* Account for precision digits ahead of the decimal point */
		for (k = 6; k >= 0 && val < powTen[k]; k--);
		precision -= k + 1;

		/* Print in decimal notation */
		sprintf(s, "%.*f", (int) precision, (double) val);

		/* Remove trailing zeroes after the decimal point */
		for (k = strlen(buf) - 1;
		    buf[k] == '0' && buf[k - 1] != '.';
		    k--) {
			buf[k] = '\0';
		}
	} else {
		char *t, *eptr, *eval;

		/* Print in exponential notation */
		sprintf(s, "%.*E", (int) precision - 1, (double) val);

		/* Find the exponent */
		eptr = strchr(buf, 'E');
		assert(eptr != NULL);

		/* Remove a '+' sign, but leave a '-' sign */
		switch (eptr[1]) {
		case '-':
			eval = &eptr[2];
			break;
		case '+':			/* remove '+' */
			memmove(eptr + 1, eptr + 2, strlen(eptr + 1));
			/* fall through */
		default:			/* shouldn't ever happen */
			eval = &eptr[1];
			break;
		}

		/* Remove leading zeroes in the exponent, if any */
		for (t = eval; t[0] == '0' && t[1] != '\0'; t++);
		memmove(eval, t, strlen(t) + 1);

		/* Remove trailing zeroes after the decimal point */
		for (t = eptr - 1; *t == '0' && t[-1] != '.'; t--);
		memmove(t + 1, eptr, strlen(eptr) + 1);
	}
	return (checkPtr(stringC2Java(buf)));
}

/*
 * Convert string to double object.
 */
double
java_lang_Double_valueOf0(struct Hjava_lang_String* str)
{
	double value;
	char buf0[MAXNUMLEN];
	char *buf;
	char* endbuf;
	char* msg = "Bad float/double format";
#if defined(STRTOD_m0_BROKEN)
	int negate;
#endif

	/* stringJava2CBuf would silently promote 0 to an empty string */
	if (str == 0) {
		SignalError("java.lang.NullPointerException", "");
	}
	stringJava2CBuf(str, buf0, sizeof(buf0));

	/* Skip initial white space */
	for (buf = buf0; isspace(*buf); buf++)
		;

#if defined(HAVE_STRTOD)

#if defined(STRTOD_m0_BROKEN)
	/* Handle negative sign manually */
	negate = (*buf == '-');
	if (negate) {
		buf++;
		if (*buf == '-') {	/* disallow double negative */
			goto bail;
		}
	}
#endif

	/* Parse value; check for empty parse */
	value = strtod(buf, &endbuf);
	if (endbuf == buf) {
		msg = buf0;		/* this is what JDK 1.1.6 does */
		goto bail;
	}

	/* Skip trailing whitespace and/or type suffixes */
	while (*endbuf != '\0') {
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

	/* Check for overflow/underflow */
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

#if defined(STRTOD_m0_BROKEN)
	if (negate) {
		value = -value;
	}
#endif

#else  /* HAVE_STRTOD */
	/* Fall back on old atof - no error checking */
	value = atof(buf);
#endif

	/* Got a good value; return it */
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
java_lang_Double_doubleToRawLongBits(jdouble val)
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

