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
#include "../../../kaffe/kaffevm/stackTrace.h"
#include <native.h>
#include "defs.h"
#include "files.h"
#include "../../../include/system.h"
#include "ObjectInputStream.h"

struct Hjava_lang_Class*
java_io_ObjectInputStream_loadClass0(struct Hjava_io_ObjectInputStream* stream, struct Hjava_lang_Class* cls, struct Hjava_lang_String* str)
{
	Hjava_lang_Class* clazz;
	char* cstr;
	Utf8Const* nm;
	Hjava_lang_ClassLoader* loader;
	int i;
	stackTraceInfo* info;

	cstr = makeCString(str);
	classname2pathname(cstr, cstr);
	nm = makeUtf8Const(cstr, -1);
	free(cstr);
	
	assert(cls == 0 || !"Don't know what to do with a non-zero class");

        loader = 0;
        info = (stackTraceInfo*)buildStackTrace(0);
        for (i = 0; info[i].meth != ENDOFSTACK; i++) {
                if (info[i].meth != 0 && info[i].meth->class != 0) {
                        loader = info[i].meth->class->loader;
                        break;
                }
        }

	if (cstr[0] == '[') {
		clazz = loadArray(nm, loader);
	}
	else {
		clazz = loadClass(nm, loader);
	}

	return (clazz);
}

void
java_io_ObjectInputStream_inputClassFields(struct Hjava_io_ObjectInputStream* stream, struct Hjava_lang_Object* obj, struct Hjava_lang_Class* cls, HArrayOfInt* arr)
{
	int i;

	assert(obj != 0 || !"Attempt to read fields into null object");

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

	/*
	 * Each subclass of a Serializable object may define its own 
	 * readObject method.  [...] When implemented, the class is only 
	 * responsible for restoring its own fields, not those of its 
	 * supertypes or subtypes. 
	 */
	meth = findMethodLocal(cls, 
		makeUtf8Const("readObject", -1), 
		makeUtf8Const("(Ljava/io/ObjectInputStream;)V", -1));

	if (meth != 0) {
		do_execute_java_method(obj, 0, 0, meth, 0, stream);
		return (true);
	}
	else {
		return (false);
	}
}
