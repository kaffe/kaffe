/*     
 * java.lang.reflect.Array.c
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
#include "object.h"
#include "classMethod.h"
#include "baseClasses.h"
#include "itypes.h"
#include "soft.h"
#include "exception.h"
#include "java_lang_reflect_Array.h"
#include <native.h>
#include "defs.h"

jint
java_lang_reflect_Array_getLength(struct Hjava_lang_Object* obj)
{
	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	return (obj_length((HArrayOfObject*)obj));
}

struct Hjava_lang_Object*
java_lang_reflect_Array_newArray(struct Hjava_lang_Class* clazz, jint size)
{
	if (size < 0) {
		SignalError("java.lang.NegativeArraySizeException", "");
	}

	return (newArray(clazz, (jsize) size));
}

