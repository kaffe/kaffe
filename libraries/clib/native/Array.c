/*     
 * java.lang.reflect.Array.c
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
#include "../../../kaffe/kaffevm/object.h"
#include "Array.h"
#include <native.h>
#include "defs.h"

jint
java_lang_reflect_Array_getLength(struct Hjava_lang_Object* obj)
{
	return (obj_length((HArrayOfObject*)obj));
}

struct Hjava_lang_Object*
java_lang_reflect_Array_get(struct Hjava_lang_Object* obj, jint elem)
{
	return (unhand((HArrayOfObject*)obj)->body[elem]);
}

jbool
java_lang_reflect_Array_getBoolean(struct Hjava_lang_Object* obj, jint elem)
{
	return (unhand((HArrayOfBoolean*)obj)->body[elem]);
}

jbyte
java_lang_reflect_Array_getByte(struct Hjava_lang_Object* obj, jint elem)
{
	return (unhand((HArrayOfByte*)obj)->body[elem]);
}

jchar
java_lang_reflect_Array_getChar(struct Hjava_lang_Object* obj, jint elem)
{
	return (unhand((HArrayOfChar*)obj)->body[elem]);
}

jshort
java_lang_reflect_Array_getShort(struct Hjava_lang_Object* obj, jint elem)
{
	return (unhand((HArrayOfShort*)obj)->body[elem]);
}

jint
java_lang_reflect_Array_getInt(struct Hjava_lang_Object* obj, jint elem)
{
	return (unhand((HArrayOfInt*)obj)->body[elem]);
}

jlong
java_lang_reflect_Array_getLong(struct Hjava_lang_Object* obj, jint elem)
{
	return (unhand((HArrayOfLong*)obj)->body[elem]);
}

jfloat
java_lang_reflect_Array_getFloat(struct Hjava_lang_Object* obj, jint elem)
{
	return (unhand((HArrayOfFloat*)obj)->body[elem]);
}

jdouble
java_lang_reflect_Array_getDouble(struct Hjava_lang_Object* obj, jint elem)
{
	return (unhand((HArrayOfDouble*)obj)->body[elem]);
}

void
java_lang_reflect_Array_set(struct Hjava_lang_Object* obj, jint elem, struct Hjava_lang_Object* val)
{
	unhand((HArrayOfObject*)obj)->body[elem] = val;
}

void
java_lang_reflect_Array_setBoolean(struct Hjava_lang_Object* obj, jint elem, jbool val)
{
	unhand((HArrayOfBoolean*)obj)->body[elem] = val;
}

void
java_lang_reflect_Array_setByte(struct Hjava_lang_Object* obj, jint elem, jbyte val)
{
	unhand((HArrayOfByte*)obj)->body[elem] = val;
}

void
java_lang_reflect_Array_setChar(struct Hjava_lang_Object* obj, jint elem, jchar val)
{
	unhand((HArrayOfChar*)obj)->body[elem] = val;
}

void
java_lang_reflect_Array_setShort(struct Hjava_lang_Object* obj, jint elem, jshort val)
{
	unhand((HArrayOfShort*)obj)->body[elem] = val;
}

void
java_lang_reflect_Array_setInt(struct Hjava_lang_Object* obj, jint elem, jint val)
{
	unhand((HArrayOfInt*)obj)->body[elem] = val;
}

void
java_lang_reflect_Array_setLong(struct Hjava_lang_Object* obj, jint elem, jlong val)
{
	unhand((HArrayOfLong*)obj)->body[elem] = val;
}

void
java_lang_reflect_Array_setFloat(struct Hjava_lang_Object* obj, jint elem, jfloat val)
{
	unhand((HArrayOfFloat*)obj)->body[elem] = val;
}

void
java_lang_reflect_Array_setDouble(struct Hjava_lang_Object* obj, jint elem, jdouble val)
{
	unhand((HArrayOfDouble*)obj)->body[elem] = val;
}

struct Hjava_lang_Object*
java_lang_reflect_Array_newArray(struct Hjava_lang_Class* clazz, jint size)
{
	return (newArray(clazz, size));
}

struct Hjava_lang_Object*
java_lang_reflect_Array_multiNewArray(struct Hjava_lang_Class* clazz, HArrayOfInt* sizes)
{
	int* dims;
	int i;
	int s;
	Hjava_lang_Object* array;

	s = obj_length(sizes);
	dims = calloc(s, sizeof(int));

	/* Copy dimentions into array */
	for( i = 0; i < s; i++ ) {
		dims[i] = unhand(sizes)->body[i];
	}
	dims[i] = 0;

	/* Create multi-dimension array */
	array = newMultiArray(clazz, dims);
	free(dims);

	return (array);
}
