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
#include "classMethod.h"
#include "gtypes.h"
#include "locks.h"
#include "stringSupport.h"
#include "fp.h"
#include "defs.h"
#include "files.h"
#include "java_lang_Double.h"
#include <native.h>

/*
 * Convert a "normal" double to a string with the supplied precision.
 * The string is stored in the supplied buffer. The buffer is
 * returned upon completion.
 *
 * This assumes printf(3) conforms to ISO 9899: 1990 (``ISO C'').
 *
 * If you fix bugs in printing methods in Double.c, please fix them
 * in Float.c as well.
 */
static char *
toCharArrayWithPrecision(char * buf, int len, jdouble val, jint precision) {
 	const jlong bits = java_lang_Double_doubleToRawLongBits(val);
	char *s;
	int k;

	/* Deal with negative numbers manually so negative zero is "-0.0" */
	s = buf;
	if (bits & DSIGNBIT) {
		val = -val;
		*s++ = '-';
		len--;
	}

	/* Print in normal or 'scientific' form according to value */
	if (val == 0.0 || (val >= 1.0e-3 && val < 1.0e7)) {
		/* Print in decimal notation */
		snprintf(s, len, "%.*f", (int) precision, (double) val);

		/* Remove trailing zeroes after the decimal point */
		for (k = strlen(buf) - 1;
		    k > 1 && buf[k] == '0' && buf[k - 1] != '.';
		    k--) {
			buf[k] = '\0';
		}
	} else {
		char *t, *eptr, *eval;

		/* Print in exponential notation */
		snprintf(s, len, "%.*E", (int) precision - 1, (double) val);

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
	return buf;
}

/*
 * Convert char array to double object.
 *
 * If you fix bugs in printing methods in Double.c, please fix them
 * in Float.c as well.
 */
static double
valueOfCharArray(char * buf)
{
	double value;
	char* startbuf;
	char* endbuf;
	char* msg = "Bad float/double format";
#if defined(STRTOD_m0_BROKEN)
	int negate;
#endif

	startbuf = buf;

	/* Skip initial white space */
	while(isspace((int) *buf)) {
	    buf++;
	}

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
		msg = startbuf;		/* this is what JDK 1.1.6 does */
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

bail:
	SignalError("java.lang.NumberFormatException", msg);
	return (0);
}

/*
 * Convert a "normal" double to a string with the supplied precision.
 *
 * This assumes printf(3) conforms to ISO 9899: 1990 (``ISO C'').
 *
 * If you fix bugs in printing methods in Double.c, please fix them
 * in Float.c as well.
 */
struct Hjava_lang_String*
java_lang_Double_toStringWithPrecision(jdouble val, jint max_precision)
{
	char buf[MAXNUMLEN];
	jint min_precision = 1;
	jint precision = 0;

	/* FreeBSD's snprintf dies on very large precision. */
	if (max_precision > MAXNUMLEN)
		max_precision = MAXNUMLEN;

	/* perform a binary search over precision. */
	while (max_precision != min_precision + 1) {
	    precision = (max_precision + min_precision) / 2;
	    toCharArrayWithPrecision(buf, MAXNUMLEN, val, (int) precision);

	    if (valueOfCharArray(buf) != val) {
		min_precision = precision;
	    }
	    else {
		max_precision = precision;
	    }
	}

	/* we could have exited the loop with
	 * precision == min_precision,
	 * where the value with min_precision
	 * is not equal to val.
	 */
	if (precision == min_precision) {
	    toCharArrayWithPrecision(buf, MAXNUMLEN, val, (int) precision + 1);
	}

	return (checkPtr(stringC2Java(buf)));
}

/*
 * Convert string to double object.
 */
double
java_lang_Double_valueOf0(struct Hjava_lang_String* str)
{
	char *buf0;
	double val;

	/* stringJava2C would silently promote 0 to an empty string */
	if (str == 0) {
		SignalError("java.lang.NullPointerException", "");
	}
	buf0 = stringJava2C(str);

	val = valueOfCharArray(buf0);
	gc_free(buf0);
	return val;
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

