/*
 * java.lang.reflect.Field.c
 *
 * Copyright (c) 1996,97 T. J. Wilkinson & Associates, London, UK.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Written by Tim Wilkinson <tim@tjwassoc.co.uk>
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "access.h"
#include "constants.h"
#include "object.h"
#include "classMethod.h"
#include "itypes.h"
#include "support.h"
#include "soft.h"
#include "baseClasses.h"
#include "exception.h"
#include "java_io_InputStream.h"
#include "java_io_PrintStream.h"
#include "java_lang_reflect_Field.h"
#include <native.h>
#include "defs.h"

static
void*
getFieldAddress(Hjava_lang_reflect_Field* this, struct Hjava_lang_Object* obj)
{
        Hjava_lang_Class* clas;
        Field* fld;

        clas = (Hjava_lang_Class*)unhand(this)->clazz;
        fld = CLASS_FIELDS(clas) + unhand(this)->slot;

        if (unhand(this)->slot < CLASS_NSFIELDS(clas)) {
		errorInfo einfo;
		if (!processClass(clas, CSTATE_COMPLETE, &einfo)) {
			throwError(&einfo);
		}
                return (FIELD_ADDRESS(fld));
        }
        else {
                if (obj == NULL) {
                        SignalError("java.lang.NullPointerException", "");
                }
                if  (!soft_instanceof(clas, obj)) {
                        SignalError("java.lang.IllegalArgumentException","");
                }
                return ((void*)(((char*)(obj)) + FIELD_BOFFSET(fld)));
        }
}

/* WHAT WITH SECURITY RESTRICTIONS !!!??? */


jint 
java_lang_reflect_Field_getModifiers(struct Hjava_lang_reflect_Field * this)
{
        Hjava_lang_Class* clas;
        Field* fld;

        clas = (Hjava_lang_Class*) unhand(this)->clazz;
        fld = CLASS_FIELDS(clas) + unhand(this)->slot;

        return ((jint)(fld->accflags & ACC_MASK));
}

struct Hjava_lang_Object*
java_lang_reflect_Field_getObject0(struct Hjava_lang_reflect_Field * this, struct Hjava_lang_Object* obj)
{
	return (*(struct Hjava_lang_Object**)getFieldAddress(this, obj));
}

jboolean
java_lang_reflect_Field_getBoolean0(struct Hjava_lang_reflect_Field * this, struct Hjava_lang_Object* obj)
{
	return (*(jboolean*)getFieldAddress(this, obj));
}

jbyte
java_lang_reflect_Field_getByte0(struct Hjava_lang_reflect_Field * this, struct Hjava_lang_Object* obj)
{
	return (*(jbyte*)getFieldAddress(this, obj));
}

jchar
java_lang_reflect_Field_getChar0(struct Hjava_lang_reflect_Field * this, struct Hjava_lang_Object* obj)
{
	return (*(jchar*)getFieldAddress(this, obj));
}


jshort
java_lang_reflect_Field_getShort0(struct Hjava_lang_reflect_Field * this, struct Hjava_lang_Object* obj)
{
	return (*(jshort*)getFieldAddress(this, obj));
}

jint
java_lang_reflect_Field_getInt0(struct Hjava_lang_reflect_Field * this, struct Hjava_lang_Object* obj)
{
	return (*(jint*)getFieldAddress(this, obj));
}

jlong
java_lang_reflect_Field_getLong0(struct Hjava_lang_reflect_Field * this, struct Hjava_lang_Object* obj)
{
	return (*(jlong*)getFieldAddress(this, obj));
}

jfloat
java_lang_reflect_Field_getFloat0(struct Hjava_lang_reflect_Field * this, struct Hjava_lang_Object* obj)
{
	return (*(jfloat*)getFieldAddress(this, obj));
}

jdouble
java_lang_reflect_Field_getDouble0(struct Hjava_lang_reflect_Field * this, struct Hjava_lang_Object* obj)
{
	return (*(jdouble*)getFieldAddress(this, obj));
}


void
java_lang_reflect_Field_setBoolean0(struct Hjava_lang_reflect_Field * this, struct Hjava_lang_Object* obj, jboolean val)
{
	*(jboolean*)getFieldAddress(this, obj) = val;
}

void
java_lang_reflect_Field_setByte0(struct Hjava_lang_reflect_Field * this, struct Hjava_lang_Object* obj, jbyte val)
{
	*(jbyte*)getFieldAddress(this, obj) = val;
}

void
java_lang_reflect_Field_setChar0(struct Hjava_lang_reflect_Field * this, struct Hjava_lang_Object* obj, jchar val)
{
	*(jchar*)getFieldAddress(this, obj) = val;
}

void
java_lang_reflect_Field_setShort0(struct Hjava_lang_reflect_Field * this, struct Hjava_lang_Object* obj, jshort val)
{
	*(jshort*)getFieldAddress(this, obj) = val;
}

void
java_lang_reflect_Field_setInt0(struct Hjava_lang_reflect_Field * this, struct Hjava_lang_Object* obj, jint val)
{
	*(jint*)getFieldAddress(this, obj) = val;
}

void
java_lang_reflect_Field_setLong0(struct Hjava_lang_reflect_Field * this, struct Hjava_lang_Object* obj, jlong val)
{
	*(jlong*)getFieldAddress(this, obj) = val;
}

void
java_lang_reflect_Field_setFloat0(struct Hjava_lang_reflect_Field * this, struct Hjava_lang_Object* obj, jfloat val)
{
	*(jfloat*)getFieldAddress(this, obj) = val;
}

void
java_lang_reflect_Field_setDouble0(struct Hjava_lang_reflect_Field * this, struct Hjava_lang_Object* obj, jdouble val)
{
	*(jdouble*)getFieldAddress(this, obj) = val;
}

void
java_lang_reflect_Field_setObject0(struct Hjava_lang_reflect_Field * this, struct Hjava_lang_Object* obj, struct Hjava_lang_Object* val)
{
	*(jobject*)getFieldAddress(this, obj) = val;
}
