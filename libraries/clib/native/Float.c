/*
 * java.lang.Float.c
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
#include <math.h>
#include "../../../kaffe/kaffevm/gtypes.h"
#include "../../../kaffe/kaffevm/classMethod.h"
#include "../../../kaffe/kaffevm/fp.h"
#include <native.h>
#include "defs.h"
#include "java_lang_Float.h"
#include "java_lang_Double.h"

/*
 * Convert float to bits.
 */
jint
java_lang_Float_floatToRawIntBits(jfloat val)
{
	return floatToInt(val);
}

/*
 * Convert bits to float.
 */
float
java_lang_Float_intBitsToFloat(jint val)
{
	return intToFloat(val);
}
