/*
 * java.io.ObjectInputStream.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "lib-license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#include "../../../kaffe/kaffevm/gtypes.h"
#include "../../../kaffe/kaffevm/classMethod.h"
#include "../../../kaffe/kaffevm/support.h"
#include "../../../kaffe/kaffevm/lookup.h"
#include <native.h>
#include "defs.h"
#include "files.h"
#include "system.h"
#include "ObjectInputStream.h"

struct Hjava_lang_Class*
java_io_ObjectInputStream_loadClass0(struct Hjava_io_ObjectInputStream* stream, struct Hjava_lang_Class* cls, struct Hjava_lang_String* str)
{
	Hjava_lang_Class* clazz;
	char* cstr;
	Utf8Const* nm;

	cstr = makeCString(str);
	classname2pathname(cstr, cstr);
	nm = makeUtf8Const(cstr, -1);
	free(cstr);
	
	clazz = loadClass(nm, 0);

	return (clazz);
}

void
java_io_ObjectInputStream_inputClassFields(struct Hjava_io_ObjectInputStream* stream, struct Hjava_lang_Object* obj, struct Hjava_lang_Class* cls, HArrayOfInt* arr)
{
	Method* meth;
	jvalue ret;
	int i;

#define	READ(FUNC,SIG,TYPE) \
	((jvalue*)(unhand(arr)->body[i+1] + (uintp)obj))->TYPE = \
		do_execute_java_method(stream, #FUNC, #SIG, 0, 0).TYPE

	for (i = 0; i < obj_length(arr); i += 2) {
		switch(unhand(arr)->body[i]) {
		case 'B':
			READ(readByte, ()B, b);
			break;
		case 'C':
			READ(readChar, ()C, c);
			break;
		case 'D':
			READ(readDouble, ()D, d);
			break;
		case 'F':
			READ(readFloat, ()F, f);
			break;
		case 'J':
			READ(readLong, ()J, j);
			break;
		case 'S':
			READ(readShort, ()S, s);
			break;
		case 'Z':
			READ(readBoolean, ()Z, z);
			break;
		case 'I':
			READ(readInt, ()I, i);
			break;
		case 'L':
		case '[':
			READ(readObject, ()Ljava/lang/Object;, l);
			break;
		default:
			SignalError("java.io.InvalidClassException", "Unknown data type");
		}
	}
#undef READ
}

struct Hjava_lang_Object*
java_io_ObjectInputStream_allocateNewObject(struct Hjava_lang_Class* cls, struct Hjava_lang_Class* cls2)
{
	return (newObject(cls));
}

struct Hjava_lang_Object*
java_io_ObjectInputStream_allocateNewArray(struct Hjava_lang_Class* cls, jint sz)
{
	if (CLASS_IS_ARRAY(cls)) {
		return (newArray(CLASS_ELEMENT_TYPE(cls), sz));
        }
	else {
		return (0);
	}
}

jbool
java_io_ObjectInputStream_invokeObjectReader(struct Hjava_io_ObjectInputStream* stream, struct Hjava_lang_Object* obj, struct Hjava_lang_Class* cls)
{
	Method* meth;
	jvalue args;

	meth = lookupClassMethod(cls, "readObject", "(Ljava/io/ObjectInputStream;)V");
	if (meth != 0) {
		do_execute_java_method(obj, 0, 0, meth, 0, stream);
		return (true);
	}
	else {
		return (false);
	}
}
