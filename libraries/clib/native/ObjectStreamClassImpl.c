/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "../../../kaffe/kaffevm/classMethod.h"
#include "../../../kaffe/kaffevm/lookup.h"
#include "../../../kaffe/kaffevm/access.h"
#include "../../../kaffe/kaffevm/stringSupport.h"
#include <native.h>
#include "java_io_ObjectInputStream.h"
#include "java_io_ObjectOutputStream.h"
#include "kaffe_io_ObjectStreamClassImpl.h"

extern jclass Kaffe_FindClass(JNIEnv*, const char*);
extern void Kaffe_ExceptionClear(JNIEnv*);
extern jobject Kaffe_NewObject(JNIEnv*, jclass, jmethodID, ...);

static Hjava_lang_Object* newSerialObject(Hjava_lang_Class*,Hjava_lang_Object*);
static HArrayOfObject* getFields(struct Hkaffe_io_ObjectStreamClassImpl*);

static Utf8Const* serialVersionUIDName;
static Utf8Const* writeObjectName;
static Utf8Const* readObjectName;
static Utf8Const* ObjectOutputStreamSig;
static Utf8Const* ObjectInputStreamSig;
static Hjava_lang_Class* ptrType;

void
kaffe_io_ObjectStreamClassImpl_init(void)
{
	errorInfo einfo;

	serialVersionUIDName = utf8ConstNew("serialVersionUID" , -1);
	writeObjectName = utf8ConstNew("writeObject", -1);
	readObjectName = utf8ConstNew("readObject", -1);
	ObjectOutputStreamSig = utf8ConstNew("(Ljava/io/ObjectOutputStream;)V", -1);
	ObjectInputStreamSig = utf8ConstNew("(Ljava/io/ObjectInputStream;)V", -1);
	ptrType = lookupClass("kaffe/util/Ptr", &einfo);
}

struct Hjava_lang_Object*
kaffe_io_ObjectStreamClassImpl_allocateNewObject(struct Hkaffe_io_ObjectStreamClassImpl* cls)
{
	return (newObject(unhand(cls)->clazz));
}

struct Hjava_lang_Object*
kaffe_io_ObjectStreamClassImpl_allocateNewArray(struct Hkaffe_io_ObjectStreamClassImpl* cls, jint sz)
{
	return (newArray(CLASS_ELEMENT_TYPE(unhand(cls)->clazz), sz));
}

void
kaffe_io_ObjectStreamClassImpl_inputClassFields(struct Hkaffe_io_ObjectStreamClassImpl* cls, struct Hjava_lang_Object* obj, struct Hjava_io_ObjectInputStream* in)
{
	int i;
	int len;
	Field** fld;

	if (unhand(cls)->fields == 0) {
		unhand(cls)->fields = getFields(cls);
	}
	fld = (Field**)unhand(unhand(cls)->fields)->body;
	len = obj_length(unhand(cls)->fields);

	if (unhand(cls)->iclazz != unhand(cls)->clazz) {
		obj = newSerialObject(unhand(cls)->iclazz, obj);
	}

#define READ(FUNC,SIG,TYPE) \
	((jvalue*)(((uint8*)obj) + FIELD_OFFSET(*fld)))->TYPE = \
		do_execute_java_method(in, #FUNC, #SIG, 0, 0).TYPE

	for (i = 0; i < len; i++, fld++) {
		if (FIELD_ISREF(*fld)) {
                        READ(readObject, ()Ljava/lang/Object;, l);
		}
		else switch (CLASS_PRIM_SIG(FIELD_TYPE(*fld))) {
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
                default:
                        SignalError("java.io.InvalidClassException", "Unknown data type");
		}
	}

	if (unhand(cls)->clazz != unhand(cls)->iclazz) {
		/* Transfer the inner class data to the outer class */
		do_execute_java_method(obj, "readDefaultObject", "()V", 0, 0);
	}
}

void
kaffe_io_ObjectStreamClassImpl_outputClassFields(struct Hkaffe_io_ObjectStreamClassImpl* cls, struct Hjava_lang_Object* obj, struct Hjava_io_ObjectOutputStream* out)
{
	int i;
	int len;
	Field** fld;

	if (unhand(cls)->fields == 0) {
		unhand(cls)->fields = getFields(cls);
	}
	fld = (Field**)unhand(unhand(cls)->fields)->body;
	len = obj_length(unhand(cls)->fields);

	if (unhand(cls)->iclazz != unhand(cls)->clazz) {
		/* Transfer the outer class data to the inner class */
		obj = newSerialObject(unhand(cls)->iclazz, obj);
		do_execute_java_method(obj, "writeDefaultObject", "()V", 0, 0);
	}

#define WRITE(FUNC,SIG,TYPE) \
	do_execute_java_method(out, #FUNC, #SIG, 0, 0, \
		((jvalue*)(((uint8*)obj) + FIELD_OFFSET(*fld)))->TYPE)

	for (i = 0; i < len; i++, fld++) {
		if (FIELD_ISREF(*fld)) {
                        WRITE(writeObject, (Ljava/lang/Object;)V, l);
		}
		else switch (CLASS_PRIM_SIG(FIELD_TYPE(*fld))) {
                case 'B':
                        WRITE(writeByte, (I)V, b);
                        break;
                case 'C':
                        WRITE(writeChar, (I)V, c);
                        break;
                case 'D':
                        WRITE(writeDouble, (D)V, d);
                        break;
                case 'F':
                        WRITE(writeFloat, (F)V, f);
                        break;
                case 'J':
                        WRITE(writeLong, (J)V, j);
                        break;
                case 'S':
                        WRITE(writeShort, (I)V, s);
                        break;
                case 'Z':
                        WRITE(writeBoolean, (Z)V, z);
                        break;
                case 'I':
                        WRITE(writeInt, (I)V, i);
                        break;
                default:
                        SignalError("java.io.InvalidClassException", "Unknown data type");
		}
	}
}

void
kaffe_io_ObjectStreamClassImpl_outputClassFieldInfo(struct Hkaffe_io_ObjectStreamClassImpl* cls, struct Hjava_io_ObjectOutputStream* out)
{
	Hjava_lang_Class* type;
	int i;
	int len;
	Field** fld;
	char* tname;
	char buf[128];

	/* If we have no fields, we must build an array of field pointers
	 * and sort them.
	 */
	if (unhand(cls)->fields == 0) {
		unhand(cls)->fields = getFields(cls);
	}
	fld = (Field**)unhand(unhand(cls)->fields)->body;
	len = obj_length(unhand(cls)->fields);

	do_execute_java_method(out, "writeShort", "(I)V", 0, 0, len);

	for (i = 0; i < len; i++, fld++) {
		type = FIELD_TYPE(*fld);
		if (CLASS_IS_PRIMITIVE(type)) {
			do_execute_java_method(out, "writeByte", "(I)V", 0, 0, CLASS_PRIM_SIG(type));
			do_execute_java_method(out, "writeUTF", "(Ljava/lang/String;)V", 0, 0, utf8Const2Java((*fld)->name));
/* We dont output the modifiers 
			do_execute_java_method(out, "writeShort", "(I)V", 0, 0, (*fld)->accflags);
*/
		}
		else {
			if (!FIELD_RESOLVED(*fld)) {
				tname = (char*)((Utf8Const*)type)->data;
			}
			else {
				tname = (char*)type->name->data;
			}
			if (tname[0] == '[') {
				strcpy(buf, tname);
			}
			else {
				strcpy(buf, "L");
				strcat(buf, tname);
				strcat(buf, ";");
			}
			do_execute_java_method(out, "writeByte", "(I)V", 0, 0, buf[0]);
			do_execute_java_method(out, "writeUTF", "(Ljava/lang/String;)V", 0, 0, utf8Const2Java((*fld)->name));
/* We dont output the modifiers 
			do_execute_java_method(out, "writeShort", "(I)V", 0, 0, (*fld)->accflags);
*/
			do_execute_java_method(out, "writeObject", "(Ljava/lang/Object;)V", 0, 0, stringC2Java(buf));
		}
	}
}

jboolean
kaffe_io_ObjectStreamClassImpl_invokeObjectReader0(struct Hkaffe_io_ObjectStreamClassImpl* cls, struct Hjava_lang_Object* obj, struct Hjava_io_ObjectInputStream* in)
{
	Method* meth;
	Hjava_lang_Object* oldObj;
	struct Hjava_io_ObjectStreamClass* oldCls;

	oldObj = unhand(in)->currentObject;
	oldCls = unhand(in)->currentStreamClass;
	unhand(in)->currentObject = obj;
	unhand(in)->currentStreamClass = (struct Hjava_io_ObjectStreamClass*)cls;

	meth = findMethodLocal(unhand(cls)->clazz, readObjectName, ObjectInputStreamSig);
	do_execute_java_method(obj, 0, 0, meth, 0, in);

	unhand(in)->currentObject = oldObj;
	unhand(in)->currentStreamClass = oldCls;

	return (true);
}

jboolean
kaffe_io_ObjectStreamClassImpl_invokeObjectWriter0(struct Hkaffe_io_ObjectStreamClassImpl* cls, struct Hjava_lang_Object* obj, struct Hjava_io_ObjectOutputStream* out)
{
	Method* meth;
	Hjava_lang_Object* oldObj;
	struct Hjava_io_ObjectStreamClass* oldCls;

	oldObj = unhand(out)->currentObject;
	oldCls = unhand(out)->currentStreamClass;
	unhand(out)->currentObject = obj;
	unhand(out)->currentStreamClass = (struct Hjava_io_ObjectStreamClass*)cls;

	meth = findMethodLocal(unhand(cls)->clazz, writeObjectName, ObjectOutputStreamSig);
	do_execute_java_method(obj, 0, 0, meth, 0, out);

	unhand(out)->currentObject = oldObj;
	unhand(out)->currentStreamClass = oldCls;

	return (true);
}

jlong
kaffe_io_ObjectStreamClassImpl_getSerialVersionUID(struct Hkaffe_io_ObjectStreamClassImpl* cls)
{
	Field* fld;
	errorInfo einfo;

	fld = lookupClassField(unhand(cls)->clazz, serialVersionUIDName, true, &einfo);
	if (fld == 0) {
		return (0L);
	}
	return (*(jlong*)FIELD_ADDRESS((Field*)fld));
}

jbool
kaffe_io_ObjectStreamClassImpl_hasWriteObject(struct Hjava_lang_Class* clazz)
{
	for (; clazz != 0; clazz = clazz->superclass) {
		Method* mptr = findMethodLocal(clazz, writeObjectName, ObjectOutputStreamSig);
		if (mptr != 0) {
			return (true);
		}
	}
	return (false);
}

static
Hjava_lang_Class*
findDefaultSerialization(Hjava_lang_Class* clazz)
{
	char* name;
	Hjava_lang_Class* nclazz;
	errorInfo einfo;

	name = KMALLOC(strlen(clazz->name->data) + 22);
	strcpy(name, clazz->name->data);
	strcat(name, "$DefaultSerialization");

	/* Use the JNI because it handles errors */
	nclazz = lookupClass(name, &einfo);
	if (nclazz == 0) {
		nclazz = clazz;
	}

	KFREE(name);

	return (nclazz);
}

/*
 * Create an instance of the inner class using the given object.  This is
 * slightly dodgy since it just finds the first <init> method which takes
 * an argument - but then there shouldn't be any others.
 */
static
Hjava_lang_Object*
newSerialObject(Hjava_lang_Class* clazz, Hjava_lang_Object* obj)
{
	int n;
	Method* mptr;

	n = CLASS_NMETHODS(clazz);
	for (mptr = CLASS_METHODS(clazz); --n >= 0; ++mptr) {
		if (strcmp(mptr->name->data, "<init>") == 0 && strcmp(mptr->signature->data, "()V") != 0) {
			return ((Hjava_lang_Object*)Kaffe_NewObject(0, clazz, (jmethodID)mptr, obj));
		}
	}

	return (0);
}

static
int
compare(const void* one, const void* two)
{
	return (strcmp((*(Field**)one)->name->data, (*(Field**)two)->name->data));
}

static
HArrayOfObject*
getFields(struct Hkaffe_io_ObjectStreamClassImpl* cls)
{
	Hjava_lang_Class* clazz;
	int offset;
	int len;
	Field* fld;
	int cnt;
	HArrayOfObject* array;
	int i;

	/* Check for the default serialization implemented as an inner-class */
	if (unhand(cls)->iclazz == 0) {
		unhand(cls)->iclazz = findDefaultSerialization(unhand(cls)->clazz);
	}
	if (unhand(cls)->iclazz == unhand(cls)->clazz) {
		clazz = unhand(cls)->clazz;
		offset = 0;
	}
	else {
		/* We skip the first inner class field since this is a
		 * pointer to the outer class.
		 */
		clazz = unhand(cls)->iclazz;
		offset = 1;
	}

	/* Count the number of fields we need to store */
	len = CLASS_NIFIELDS(clazz) - offset;
	fld = CLASS_IFIELDS(clazz) + offset;
	cnt = 0;
	for (i = 0; i < len; i++, fld++) {
		if ((fld->accflags & ACC_TRANSIENT) != 0) {
			continue;
		}
		cnt++;
	}

	/* Build an array of those fields */
	array = (HArrayOfObject*)newArray(ptrType, cnt);
	cnt = 0;
	fld = CLASS_IFIELDS(clazz) + offset;
	for (i = 0; i < len; i++, fld++) {
		if ((fld->accflags & ACC_TRANSIENT) != 0) {
			continue;
		}
		unhand(array)->body[cnt] = (void*)fld;
		cnt++;
	}

	/* Sort the array */
	qsort(unhand(array)->body, cnt, sizeof(void*), compare);

	return (array);
}
