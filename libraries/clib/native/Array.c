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
#include "../../../kaffe/kaffevm/classMethod.h"
#include "../../../kaffe/kaffevm/baseClasses.h"
#include "../../../kaffe/kaffevm/itypes.h"
#include "../../../kaffe/kaffevm/soft.h"
#include "Array.h"
#include "Boolean.h"
#include "Byte.h"
#include "Character.h"
#include "Short.h"
#include "Integer.h"
#include "Long.h"
#include "Float.h"
#include "Double.h"
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
java_lang_reflect_Array_get(struct Hjava_lang_Object* obj, jint elem)
{
	Hjava_lang_Class *clazz;

	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	clazz = CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj));
	
	if ( clazz == intClass ) {
		HArrayOfInt *arr = (HArrayOfInt *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return execute_java_constructor(0,javaLangIntegerClass,"(I)V",
						(jint)(unhand(arr)->body[elem]));
	}
	else if ( clazz == shortClass ) {
		HArrayOfShort *arr = (HArrayOfShort *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return execute_java_constructor(0,javaLangShortClass,"(S)V",
						(jint)(unhand(arr)->body[elem]));
	}
	else if ( clazz == longClass ) {
		HArrayOfLong *arr = (HArrayOfLong *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return execute_java_constructor(0,javaLangLongClass,"(J)V",
						(jlong)(unhand(arr)->body[elem]));
	}
	else if ( clazz == byteClass ) {
		HArrayOfByte *arr = (HArrayOfByte *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return execute_java_constructor(0,javaLangByteClass,"(B)V",
						(jint)(unhand(arr)->body[elem]));
	}
	else if ( clazz == booleanClass ) {
		HArrayOfBoolean *arr = (HArrayOfBoolean *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return execute_java_constructor(0,javaLangBooleanClass,"(Z)V",
						(jint)(unhand(arr)->body[elem]));
	}
	else if ( clazz == charClass ) {
		HArrayOfChar *arr = (HArrayOfChar *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return execute_java_constructor(0,javaLangCharacterClass,"(C)V",
						(jint)(unhand(arr)->body[elem]));
	}
	else if ( clazz == floatClass ) {
		HArrayOfFloat *arr = (HArrayOfFloat *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return execute_java_constructor(0,javaLangFloatClass,"(F)V",
						(jfloat)(unhand(arr)->body[elem]));
	}
	else if ( clazz == doubleClass ) {
		HArrayOfDouble *arr = (HArrayOfDouble *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return execute_java_constructor(0,javaLangDoubleClass,"(D)V",
						(jdouble)(unhand(arr)->body[elem]));
	}
	else {
		/* If clazz isn't one of the above then it's either a
		 * reference or unresolved (and so a reference).
		 */

		HArrayOfObject *arr = (HArrayOfObject *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	}
}

jbool
java_lang_reflect_Array_getBoolean(struct Hjava_lang_Object* obj, jint elem)
{
	Hjava_lang_Class *clazz;

	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	clazz = CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj));
	
	if ( clazz == booleanClass ) {
		HArrayOfBoolean *arr = (HArrayOfBoolean *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else {
		SignalError("java.lang.IllegalArgumentException", "");
	}
}

jbyte
java_lang_reflect_Array_getByte(struct Hjava_lang_Object* obj, jint elem)
{
	Hjava_lang_Class *clazz;

	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	clazz = CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj));
	
	if ( clazz == byteClass ) {
		HArrayOfByte *arr = (HArrayOfByte *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else {
		SignalError("java.lang.IllegalArgumentException", "");
	}
}

jchar
java_lang_reflect_Array_getChar(struct Hjava_lang_Object* obj, jint elem)
{
	Hjava_lang_Class *clazz;

	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	clazz = CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj));
	
	if ( clazz == charClass ) {
		HArrayOfChar *arr = (HArrayOfChar *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == byteClass ) {
		HArrayOfByte *arr = (HArrayOfByte *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else {
		SignalError("java.lang.IllegalArgumentException", "");
	}
}

jshort
java_lang_reflect_Array_getShort(struct Hjava_lang_Object* obj, jint elem)
{
	Hjava_lang_Class *clazz;

	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	clazz = CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj));
	
	if ( clazz == shortClass ) {
		HArrayOfShort *arr = (HArrayOfShort *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == byteClass ) {
		HArrayOfByte *arr = (HArrayOfByte *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else {
		SignalError("java.lang.IllegalArgumentException", "");
	}
}

jint
java_lang_reflect_Array_getInt(struct Hjava_lang_Object* obj, jint elem)
{
	Hjava_lang_Class *clazz;

	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	clazz = CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj));
	
	if ( clazz == intClass ) {
		HArrayOfInt *arr = (HArrayOfInt *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == shortClass ) {
		HArrayOfShort *arr = (HArrayOfShort *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == charClass ) {
		HArrayOfChar *arr = (HArrayOfChar *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == byteClass ) {
		HArrayOfByte *arr = (HArrayOfByte *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else {
		SignalError("java.lang.IllegalArgumentException", "");
	}
}

jlong
java_lang_reflect_Array_getLong(struct Hjava_lang_Object* obj, jint elem)
{
	Hjava_lang_Class *clazz;

	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	clazz = CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj));
	
	if ( clazz == longClass ) {
		HArrayOfLong *arr = (HArrayOfLong *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == intClass ) {
		HArrayOfInt *arr = (HArrayOfInt *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == shortClass ) {
		HArrayOfShort *arr = (HArrayOfShort *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == charClass ) {
		HArrayOfChar *arr = (HArrayOfChar *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == byteClass ) {
		HArrayOfByte *arr = (HArrayOfByte *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else {
		SignalError("java.lang.IllegalArgumentException", "");
	}
}

jfloat
java_lang_reflect_Array_getFloat(struct Hjava_lang_Object* obj, jint elem)
{
	Hjava_lang_Class *clazz;

	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	clazz = CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj));
	
	if ( clazz == floatClass ) {
		HArrayOfFloat *arr = (HArrayOfFloat *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == longClass ) {
		HArrayOfLong *arr = (HArrayOfLong *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == intClass ) {
		HArrayOfInt *arr = (HArrayOfInt *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == shortClass ) {
		HArrayOfShort *arr = (HArrayOfShort *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == charClass ) {
		HArrayOfChar *arr = (HArrayOfChar *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == byteClass ) {
		HArrayOfByte *arr = (HArrayOfByte *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else {
		SignalError("java.lang.IllegalArgumentException", "");
	}
}

jdouble
java_lang_reflect_Array_getDouble(struct Hjava_lang_Object* obj, jint elem)
{
	Hjava_lang_Class *clazz;

	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	clazz = CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj));
	
	if ( clazz == doubleClass ) {
		HArrayOfDouble *arr = (HArrayOfDouble *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == floatClass ) {
		HArrayOfFloat *arr = (HArrayOfFloat *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == longClass ) {
		HArrayOfLong *arr = (HArrayOfLong *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == intClass ) {
		HArrayOfInt *arr = (HArrayOfInt *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == shortClass ) {
		HArrayOfShort *arr = (HArrayOfShort *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == charClass ) {
		HArrayOfChar *arr = (HArrayOfChar *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else if ( clazz == byteClass ) {
		HArrayOfByte *arr = (HArrayOfByte *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		return unhand(arr)->body[elem];
	} else {
		SignalError("java.lang.IllegalArgumentException", "");
	}
}

void
java_lang_reflect_Array_set(struct Hjava_lang_Object* obj, jint elem, struct Hjava_lang_Object* val)
{
	Hjava_lang_Class *clazz;

	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	if (!CLASS_IS_PRIMITIVE(CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj)))) {
		HArrayOfObject *arr = (HArrayOfObject *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		if (val == NULL || soft_instanceof(CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj)), val)) {
			unhand(arr)->body[elem] = val;
		} else {
			SignalError("java.lang.IllegalArgumentException", "");
		}
		return;
	}

	clazz = OBJECT_CLASS(val);
	
	if ( clazz == javaLangIntegerClass ) {
		java_lang_reflect_Array_setInt(obj, elem, unhand(((Hjava_lang_Integer *)val))->value );
	}
	else if ( clazz == javaLangBooleanClass ) {
		java_lang_reflect_Array_setBoolean(obj, elem, unhand(((Hjava_lang_Boolean *)val))->value );
	}
	else if ( clazz == javaLangByteClass ) {
		java_lang_reflect_Array_setByte(obj, elem, unhand(((Hjava_lang_Byte *)val))->value );
	}
	else if ( clazz == javaLangShortClass ) {
		java_lang_reflect_Array_setShort(obj, elem, unhand(((Hjava_lang_Short *)val))->value );
	}
	else if ( clazz == javaLangCharacterClass ) {
		java_lang_reflect_Array_setChar(obj, elem, unhand(((Hjava_lang_Character *)val))->value );
	}
	else if ( clazz == javaLangLongClass ) {
		java_lang_reflect_Array_setLong(obj, elem, unhand(((Hjava_lang_Long *)val))->value );
	}
	else if ( clazz == javaLangFloatClass ) {
		java_lang_reflect_Array_setFloat(obj, elem, unhand(((Hjava_lang_Float *)val))->value );
	}
	else if ( clazz == javaLangDoubleClass ) {
		java_lang_reflect_Array_setDouble(obj, elem, unhand(((Hjava_lang_Double *)val))->value );
	} else {
		SignalError("java.lang.IllegalArgumentException", "");
      	}
}

void
java_lang_reflect_Array_setBoolean(struct Hjava_lang_Object* obj, jint elem, jbool val)
{
	Hjava_lang_Class *clazz;

	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	clazz = CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj));
	
	if ( clazz == booleanClass ) {
		HArrayOfBoolean *arr = (HArrayOfBoolean *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else {
		SignalError("java.lang.IllegalArgumentException", "");
	}
}

void
java_lang_reflect_Array_setByte(struct Hjava_lang_Object* obj, jint elem, jbyte val)
{
	Hjava_lang_Class *clazz;

	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	clazz = CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj));
	
	if ( clazz == byteClass ) {
		HArrayOfByte *arr = (HArrayOfByte *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == shortClass ) {
		HArrayOfShort *arr = (HArrayOfShort *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == charClass ) {
		HArrayOfChar *arr = (HArrayOfChar *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == intClass ) {
		HArrayOfInt *arr = (HArrayOfInt *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == longClass ) {
		HArrayOfLong *arr = (HArrayOfLong *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == floatClass ) {
		HArrayOfFloat *arr = (HArrayOfFloat *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == doubleClass ) {
		HArrayOfDouble *arr = (HArrayOfDouble *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else {
		SignalError("java.lang.IllegalArgumentException", "");
	}
}

void
java_lang_reflect_Array_setChar(struct Hjava_lang_Object* obj, jint elem, jchar val)
{
	Hjava_lang_Class *clazz;

	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	clazz = CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj));
	
	if ( clazz == charClass ) {
		HArrayOfChar *arr = (HArrayOfChar *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == intClass ) {
		HArrayOfInt *arr = (HArrayOfInt *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == longClass ) {
		HArrayOfLong *arr = (HArrayOfLong *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == floatClass ) {
		HArrayOfFloat *arr = (HArrayOfFloat *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == doubleClass ) {
		HArrayOfDouble *arr = (HArrayOfDouble *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else {
		SignalError("java.lang.IllegalArgumentException", "");
	}
}

void
java_lang_reflect_Array_setShort(struct Hjava_lang_Object* obj, jint elem, jshort val)
{
	Hjava_lang_Class *clazz;

	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	clazz = CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj));
	
	if ( clazz == shortClass ) {
		HArrayOfShort *arr = (HArrayOfShort *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == intClass ) {
		HArrayOfInt *arr = (HArrayOfInt *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == longClass ) {
		HArrayOfLong *arr = (HArrayOfLong *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == floatClass ) {
		HArrayOfFloat *arr = (HArrayOfFloat *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == doubleClass ) {
		HArrayOfDouble *arr = (HArrayOfDouble *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else {
		SignalError("java.lang.IllegalArgumentException", "");
	}
}

void
java_lang_reflect_Array_setInt(struct Hjava_lang_Object* obj, jint elem, jint val)
{
	Hjava_lang_Class *clazz;

	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	clazz = CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj));
	
	if ( clazz == intClass ) {
		HArrayOfInt *arr = (HArrayOfInt *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == longClass ) {
		HArrayOfLong *arr = (HArrayOfLong *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == floatClass ) {
		HArrayOfFloat *arr = (HArrayOfFloat *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == doubleClass ) {
		HArrayOfDouble *arr = (HArrayOfDouble *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else {
		SignalError("java.lang.IllegalArgumentException", "");
	}
}

void
java_lang_reflect_Array_setLong(struct Hjava_lang_Object* obj, jint elem, jlong val)
{
	Hjava_lang_Class *clazz;

	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	clazz = CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj));
	
	if ( clazz == longClass ) {
		HArrayOfLong *arr = (HArrayOfLong *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == floatClass ) {
		HArrayOfFloat *arr = (HArrayOfFloat *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == doubleClass ) {
		HArrayOfDouble *arr = (HArrayOfDouble *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else {
		SignalError("java.lang.IllegalArgumentException", "");
	}
}

void
java_lang_reflect_Array_setFloat(struct Hjava_lang_Object* obj, jint elem, jfloat val)
{
	Hjava_lang_Class *clazz;

	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	clazz = CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj));
	
	if ( clazz == floatClass ) {
		HArrayOfFloat *arr = (HArrayOfFloat *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else if ( clazz == doubleClass ) {
		HArrayOfDouble *arr = (HArrayOfDouble *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else {
		SignalError("java.lang.IllegalArgumentException", "");
	}
}

void
java_lang_reflect_Array_setDouble(struct Hjava_lang_Object* obj, jint elem, jdouble val)
{
	Hjava_lang_Class *clazz;

	if (!CLASS_IS_ARRAY(OBJECT_CLASS(obj)))
		SignalError("java.lang.IllegalArgumentException", "");

	clazz = CLASS_ELEMENT_TYPE(OBJECT_CLASS(obj));
	
	if ( clazz == doubleClass ) {
		HArrayOfDouble *arr = (HArrayOfDouble *)obj;
		if (elem < 0 || elem >= obj_length(arr))
			SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
		
		unhand(arr)->body[elem] = val;
	} else {
		SignalError("java.lang.IllegalArgumentException", "");
	}
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
	dims = calloc(s+1, sizeof(int));

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
