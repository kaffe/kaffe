/*
 * java.io.ObjectStreamClass.c
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
#include "../../../kaffe/kaffevm/classMethod.h"
#include "../../../kaffe/kaffevm/support.h"
#include "../../../kaffe/kaffevm/lookup.h"
#include "../../../kaffe/kaffevm/access.h"
#include <native.h>
#include "defs.h"
#include "files.h"
#include "system.h"
#include "ObjectStreamClass.h"
#include "ObjectStreamField.h"
#include "Field.h"

jint
java_io_ObjectStreamClass_getClassAccess(struct Hjava_lang_Class* cls)
{
	return (cls->accflags & ACC_MASK);
}

HArrayOfObject*
java_io_ObjectStreamClass_getMethodSignatures(struct Hjava_lang_Class* cls)
{
	int sz;
	int i;
	HArrayOfObject* ss;
	Method* meth;
	char buf[200];

	sz = CLASS_NMETHODS(cls);
	meth = CLASS_METHODS(cls);
	ss = (HArrayOfObject*)AllocObjectArray(sz, "Ljava/lang/String;");
	for (i = 0; i < sz; i++, meth++) {
		strcpy(buf, meth->name->data);
		strcat(buf, " ");
		strcat(buf, meth->signature->data);
		unhand(ss)->body[i] = (Hjava_lang_Object*)makeJavaString(buf, strlen(buf));
	}
	return (ss);
}

jint
java_io_ObjectStreamClass_getMethodAccess(struct Hjava_lang_Class* cls, struct Hjava_lang_String* str)
{
	return (0);	/* Definately not - XXX */
}

HArrayOfObject*
java_io_ObjectStreamClass_getFieldSignatures(struct Hjava_lang_Class* cls)
{
	int sz;
	int i;
	HArrayOfObject* ss;
	Field* fld;
	char buf[200];

	sz = CLASS_NIFIELDS(cls);
	fld = CLASS_IFIELDS(cls);
	ss = (HArrayOfObject*)AllocObjectArray(sz, "Ljava/lang/String;");
	for (i = 0; i < sz; i++, fld++) {
		strcpy(buf, fld->name->data);
		strcat(buf, " ");
		if (!FIELD_RESOLVED(fld)) {
			strcat(buf, ((Utf8Const*)FIELD_TYPE(fld))->data);
		}
		else {
			strcat(buf, FIELD_TYPE(fld)->name->data);
		}
		unhand(ss)->body[i] = (Hjava_lang_Object*)makeJavaString(buf, strlen(buf));
	}
	return (ss);
}

jint
java_io_ObjectStreamClass_getFieldAccess(struct Hjava_lang_Class* cls, struct Hjava_lang_String* str)
{
	return (0);	/* Definately not - XXX */
}

HArrayOfObject*
java_io_ObjectStreamClass_getFields0(struct Hjava_io_ObjectStreamClass* stream, struct Hjava_lang_Class* cls)
{
	int sz;
	int i;
	int cnt;
	HArrayOfObject* sf;
	Hjava_io_ObjectStreamField* obj;
	Field* fld;
	char buf[200];
	char* type;

	sz = CLASS_NIFIELDS(cls);
	cnt = 0;

	/* count how many non-transient fields there are */
	fld = CLASS_IFIELDS(cls);
	for (i = 0; i < sz; i++, fld++) {
		if ((fld->accflags & ACC_TRANSIENT) == 0)
			cnt++;
	}

	fld = CLASS_IFIELDS(cls);
	sf = (HArrayOfObject*)AllocObjectArray(cnt, "Ljava/io/ObjectStreamField;");
	for (i = 0; i < cnt; fld++) {
		/* skip transient fields */
		if (fld->accflags & ACC_TRANSIENT)
			continue;

		unhand(sf)->body[i] = AllocObject("java/io/ObjectStreamField");
		obj = (Hjava_io_ObjectStreamField*)unhand(sf)->body[i];
		unhand(obj)->name = Utf8Const2JavaString(fld->name);
		unhand(obj)->offset = FIELD_OFFSET(fld);
		if (CLASS_IS_PRIMITIVE(FIELD_TYPE(fld))) {
			unhand(obj)->type = CLASS_PRIM_SIG(FIELD_TYPE(fld));
			unhand(obj)->typeString = 0;
		}
		else {
			buf[0] = 0;
			if (!FIELD_RESOLVED(fld)) {
				type = ((Utf8Const*)FIELD_TYPE(fld))->data;
				unhand(obj)->type = type[0];
				strcat(buf, type);
			}
			else {
				type = FIELD_TYPE(fld)->name->data;
				if (type[0] =='[') {
					unhand(obj)->type = '[';
					strcat(buf, type);
				}
				else {
					unhand(obj)->type = 'L';
					strcat(buf, "L");
					strcat(buf, type);
					strcat(buf, ";");
				}
			}
			unhand(obj)->typeString = makeJavaString(buf, strlen(buf));
		}
		i++;
	}
	return (sf);
}

jlong
java_io_ObjectStreamClass_getSerialVersionUID(struct Hjava_lang_Class* cls)
{
	Field* fld;

	fld = lookupClassField(cls, makeUtf8Const("serialVersionUID" , -1), true);
	if (fld == 0) {
		/* return (0); */
		return (0x1234);
	}
	return (*(jlong*)FIELD_ADDRESS((Field*)fld));
}

jbool
java_io_ObjectStreamClass_hasWriteObject(struct Hjava_lang_Class* cls)
{
	Method* meth;   

	meth = findMethod(cls, makeUtf8Const("writeObject", -1),
		makeUtf8Const("(Ljava/io/ObjectOutputStream;)V", -1));
	if (meth != 0) {
		return (true);
	}
	else {
		return (false);
	}
}
