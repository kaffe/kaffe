/*
 * ObjectInputStream.c
 * 
 * Copyright (C) 2003, 2004 Kaffe.org's team. 
 *
 * See "licence.terms" for information on usage and redistribution 
 * of this file.
 */
#include "config.h"
#include "config-std.h"
#include <kaffe/jtypes.h>
#include <native.h>
#include "java_io_ObjectStreamField.h"
#include "../../../kaffe/kaffevm/object.h"
#include "../../../kaffe/kaffevm/support.h"

void java_io_ObjectStreamField_setBooleanNative
                (struct Hjava_io_ObjectStreamField* sfield,
		 struct Hjava_lang_Object* obj, 
		 jboolean value)
{
  do_execute_java_method(unhand(sfield)->field, "setBooleanInternal",
			 "(Ljava/lang/Object;Z)V",
			 0, false, obj, value);
}

void java_io_ObjectStreamField_setByteNative
                (struct Hjava_io_ObjectStreamField* sfield, 
		 struct Hjava_lang_Object* obj,
		 jbyte value)
{
  do_execute_java_method(unhand(sfield)->field, "setByteInternal",
			 "(Ljava/lang/Object;B)V",
			 0, false, obj, value);
}

void java_io_ObjectStreamField_setCharNative
                (struct Hjava_io_ObjectStreamField* sfield, 
		 struct Hjava_lang_Object* obj,
		 jchar value)
{
  do_execute_java_method(unhand(sfield)->field, "setCharInternal",
			 "(Ljava/lang/Object;B)V",
			 0, false, obj, value);
}

void java_io_ObjectStreamField_setShortNative
                (struct Hjava_io_ObjectStreamField* sfield, 
		 struct Hjava_lang_Object* obj,
		 jshort value)
{
  do_execute_java_method(unhand(sfield)->field, "setShortInternal",
			 "(Ljava/lang/Object;S)V",
			 0, false, obj, value);
}

void java_io_ObjectStreamField_setIntNative
                (struct Hjava_io_ObjectStreamField* sfield, 
		 struct Hjava_lang_Object* obj,
		 jint value)
{
  do_execute_java_method(unhand(sfield)->field, "setIntInternal",
			 "(Ljava/lang/Object;I)V",
			 0, false, obj, value);
}

void java_io_ObjectStreamField_setLongNative
                (struct Hjava_io_ObjectStreamField* sfield, 
		 struct Hjava_lang_Object* obj,
		 jlong value)
{
  do_execute_java_method(unhand(sfield)->field, "setLongInternal",
			 "(Ljava/lang/Object;J)V",
			 0, false, obj, value);
}

void java_io_ObjectStreamField_setFloatNative
                (struct Hjava_io_ObjectStreamField* sfield, 
		 struct Hjava_lang_Object* obj,
		 jfloat value)
{
  do_execute_java_method(unhand(sfield)->field, "setFloatInternal",
			 "(Ljava/lang/Object;F)V",
			 0, false, obj, value);
}

void java_io_ObjectStreamField_setDoubleNative
                (struct Hjava_io_ObjectStreamField* sfield, 
		 struct Hjava_lang_Object* obj,
		 jdouble value)
{
  do_execute_java_method(unhand(sfield)->field, "setDoubleInternal",
			 "(Ljava/lang/Object;D)V",
			 0, false, obj, value);
}

void java_io_ObjectStreamField_setObjectNative
                (struct Hjava_io_ObjectStreamField* sfield,
		 struct Hjava_lang_Object* obj, 
		 struct Hjava_lang_Object* value)
{
  do_execute_java_method(unhand(sfield)->field, "setInternal",
			 "(Ljava/lang/Object;Ljava/lang/Object;)V",
			 0, false, obj, value);
}
