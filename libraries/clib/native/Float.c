/*
 * java.lang.Float.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "lib-license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include <math.h>
#include "../../../kaffe/kaffevm/gtypes.h"
#include "../../../kaffe/kaffevm/classMethod.h"
#include <native.h>
#include "defs.h"
#include "Float.h"

/*
 * Convert float into a string.
 */
struct Hjava_lang_String*
java_lang_Float_toString(jfloat val)
{
	char str[MAXNUMLEN];

	sprintf(str, "%g", val);
	return (makeJavaString(str, strlen(str)));
}

struct Hjava_lang_Double;
extern double java_lang_Double_valueOf0(struct Hjava_lang_String*);

/*
 * Convert string to float object. (JDK 1.0.2)
 */
struct Hjava_lang_Float*
java_lang_Float_valueOf(struct Hjava_lang_String* str)
{
	struct Hjava_lang_Float* obj;
	obj = (struct Hjava_lang_Float*)execute_java_constructor("java.lang.Float", 0, "()V");
	unhand(obj)->value = java_lang_Double_valueOf0(str);
	return (obj);
}

/*
 * Convert float to bits.
 */
jint
java_lang_Float_floatToIntBits(jfloat val)
{
        jvalue d;
	d.f = val;
	return d.i;
}

/*
 * Convert bits to float.
 */
float
java_lang_Float_intBitsToFloat(jint val)
{
        jvalue d;
	d.i = val;
	return d.f;
}
