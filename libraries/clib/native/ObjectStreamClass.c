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
#include "../../../include/system.h"
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
	char buf[256];		/* XXX FIXED SIZE BUFFER */

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
	int sz;
	int i;
	Method* meth;
	char* name;
	char* sig;

	/* 
	 * the string `str' that is passed in will be one of the strings
	 * we returned in getMethodSignatures.  We will find the name of the
	 * method before the space and the signature after the space
	 */
	name = sig = makeCString(str);

	/* replace " " with '0' */
	while (*sig != ' ')
		sig++;
	*sig++ = '\0';

	sz = CLASS_NMETHODS(cls);
	meth = CLASS_METHODS(cls);
	for (i = 0; i < sz; i++, meth++) {
		if (strcmp(meth->name->data, name))
			continue;
		if (!strcmp(meth->signature->data, sig))
			break;
	}
	assert(i < sz || !"Method not found");

	gc_free_fixed(name);
	return (meth->accflags & ACC_MASK);
}

/*
 * Write the type of a field in a buffer as a Utf8Const and return the 
 * first character of that buffer, which denotes its type.
 * 
 * If the field is a primitive type, give its primitive signature.
 * If the field is not resolved, we can simply use the name.
 * If it was resolved, we must prepend an 'L' or '[' as appropriate.
 *
 * We must return the descriptor of a Field in UTF encoding.
 */
static char
convertFieldTypeToString(Field* fld, char *buf)
{
	char* type;

	if (CLASS_IS_PRIMITIVE(FIELD_TYPE(fld))) {
		buf[0] = CLASS_PRIM_SIG(FIELD_TYPE(fld));
		buf[1] = 0;
	} else 
	if (!FIELD_RESOLVED(fld)) {
		type = ((Utf8Const*)FIELD_TYPE(fld))->data;
		strcpy(buf, type);
	}
	else {
		type = FIELD_TYPE(fld)->name->data;
		if (type[0] =='[') {
			strcpy(buf, type);
		}
		else {
			strcpy(buf, "L");
			strcat(buf, type);
			strcat(buf, ";");
		}
	}
	return buf[0];
}

HArrayOfObject*
java_io_ObjectStreamClass_getFieldSignatures(struct Hjava_lang_Class* cls)
{
	int sz;
	int i;
	HArrayOfObject* ss;
	Field* fld;
	char buf[256];		/* XXX FIXED SIZE BUFFER */

	sz = CLASS_NFIELDS(cls);
	fld = CLASS_FIELDS(cls);
	ss = (HArrayOfObject*)AllocObjectArray(sz, "Ljava/lang/String;");
	for (i = 0; i < sz; i++, fld++) {
		strcpy(buf, fld->name->data);
		strcat(buf, " ");
		convertFieldTypeToString(fld, buf + strlen(buf));
		unhand(ss)->body[i] = (Hjava_lang_Object*)makeJavaString(buf, strlen(buf));
	}
	return (ss);
}

jint
java_io_ObjectStreamClass_getFieldAccess(struct Hjava_lang_Class* cls, struct Hjava_lang_String* str)
{
	Field* fld;
	char* name;
	char* sig;
	int i;
	int sz;

	/* 
	 * the string `str' that is passed in will be one of the strings
	 * we returned in getFieldSignatures.  We will find the name of the
	 * field before the space and its signature after the space
	 */
	name = sig = makeCString(str);

	/* replace " " with '0' */
	while (*sig != ' ')
		sig++;
	*sig++ = '\0';

	sz = CLASS_NFIELDS(cls);
	fld = CLASS_FIELDS(cls);

	for (i = 0; i < sz; i++, fld++) {
		char fsig[256];		/* XXX FIXED SIZE BUFFER */
		if (strcmp(name, fld->name->data))
			continue;
		
		convertFieldTypeToString(fld, fsig);
		if (!strcmp(sig, fsig))
			break;
	}
	assert (i < sz || !"Field not found");

	gc_free_fixed(name);
	return (fld->accflags & ACC_MASK);
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
	char buf[256];		/* XXX FIXED SIZE BUFFER */

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
		unhand(obj)->type = convertFieldTypeToString(fld, buf);

		/* set typeString if not primitive */
		if (!CLASS_IS_PRIMITIVE(FIELD_TYPE(fld))) {
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
		return (0L);
	}
	return (*(jlong*)FIELD_ADDRESS((Field*)fld));
}

jbool
java_io_ObjectStreamClass_hasWriteObject(struct Hjava_lang_Class* class)
{
	Utf8Const* name;
	Utf8Const* signature;

	/* 
	 * Note that we do not use findMethod because that would resolve
	 * the constants in this class, which is not necessary.
	 */
	name = makeUtf8Const("writeObject", -1);
	signature = makeUtf8Const("(Ljava/io/ObjectOutputStream;)V", -1);

        for (; class != 0; class = class->superclass) {
                Method* mptr = findMethodLocal(class, name, signature);
                if (mptr != NULL) {
                        return (true);
                }
        }
	return (false);
}
