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
#include "config-net.h"
#include "../../../kaffe/kaffevm/classMethod.h"
#include "../../../kaffe/kaffevm/lookup.h"
#include "../../../kaffe/kaffevm/access.h"
#include "../../../kaffe/kaffevm/stringSupport.h"
#include "../../../kaffe/kaffevm/support.h"
#include <native.h>
#include "java_io_ObjectInputStream.h"
#include "java_io_ObjectOutputStream.h"
#include "kaffe_io_ObjectStreamClassImpl.h"
#include "sha-1.h"

extern jobject Kaffe_NewObject(JNIEnv*, jclass, jmethodID, ...);   /* XXX */

static Hjava_lang_Object* newSerialObject(Hjava_lang_Class*,Hjava_lang_Object*);
static HArrayOfObject* getFields(struct Hkaffe_io_ObjectStreamClassImpl*);
static char* getClassName(Hjava_lang_Class* cls);

/* NB: these guys are all write once and then immutable */
static Utf8Const* serialVersionUIDName;
static Utf8Const* writeObjectName;
static Utf8Const* readObjectName;
static Utf8Const* ObjectOutputStreamSig;
static Utf8Const* ObjectInputStreamSig;
static Hjava_lang_Class* ptrType;

/*
 * Used to hold a descriptor item while calculating the serialUID.
 */
typedef struct {
	const char*	name;
	const char*	desc;
	int		modifier;
} uidItem;


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
	fld = (Field**)unhand_array(unhand(cls)->fields)->body;
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
	fld = (Field**)unhand_array(unhand(cls)->fields)->body;
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
	fld = (Field**)unhand_array(unhand(cls)->fields)->body;
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

static
int
compareUidItem(const void* one, const void* two)
{
	int r;
	uidItem* o = (uidItem*)one;
	uidItem* t = (uidItem*)two;

	/* Null entries are not considered */
	if (o->name == 0 || t->name == 0) {
		return (0);
	}

	r = strcmp(o->name, t->name);
	if (r == 0) {
		r = strcmp(o->desc, t->desc);
	}
	return (r);
}

static
void
addToSHA(SHA1_CTX* c, uidItem* base, int len)
{
	int i;
	int mod;
	jshort tmp;

	/* Sort the items into the required order */
	if (len > 1) {
		qsort(base, len, sizeof(uidItem), compareUidItem);
	}

	/* Now enter the data into the SHA */
	for (i = 0; i < len; i++) {
		if (base[i].name != 0) {
			tmp = htons(strlen(base[i].name));
			SHA1Update(c, (char*)&tmp, sizeof(tmp));
			SHA1Update(c, base[i].name, strlen(base[i].name));
			if (base[i].modifier != -1) {
				/* Java is in 'network' order - bad but handy */
				mod = htonl(base[i].modifier & ACC_MASK);
				SHA1Update(c, (char*)&mod, sizeof(mod));
				tmp = htons(strlen(base[i].desc));
				SHA1Update(c, (char*)&tmp, sizeof(tmp));
				SHA1Update(c, base[i].desc, strlen(base[i].desc));
			}
		}
	}
}

/*
 * Take a class name of the form pkg/subpkg/name, return a newly
 * allocated one of the form pkg.subpkg.name.
 * Caller must free using KFREE.
 */
static char*
pathname2ClassnameCopy(const char *orig)
{
	char* str;
	str = KMALLOC(strlen(orig) + 1);
	pathname2classname(orig, str);
	return (str);
}

static
const char*
getFieldDesc(Field* fld)
{
	if (!FIELD_RESOLVED(fld)) {
		/* XXX: is this right?  Is it dotted as opposed to slashed? */
		return (((Utf8Const*)(void*)fld->type)->data);
	}
	else if (!CLASS_IS_PRIMITIVE(FIELD_TYPE(fld))) {
		/* XXX: should that be pathname2ClassnameCopy ??? */
		return (FIELD_TYPE(fld)->name->data);
	}
	else {
		return (CLASS_PRIM_NAME(FIELD_TYPE(fld))->data);
	}
}

static
char*
getMethodDesc(Method* mth)
{
	return (pathname2ClassnameCopy(mth->signature->data));
}

/*
 * get this class's name in the form pkg.subpkg.name
 */
static
char*
getClassName(Hjava_lang_Class* cls)
{
	return (pathname2ClassnameCopy(cls->name->data));
}

jlong
kaffe_io_ObjectStreamClassImpl_getSerialVersionUID0(Hjava_lang_Class* cls)
{
	Field* fld;
	Method* mth;
	SHA1_CTX c;
	unsigned char md[SHA_DIGEST_LENGTH];
	int mod;
	int i;
	uidItem* base = 0;
	int len;
	errorInfo einfo;
	jshort tmp;
	char *classname;

	fld = lookupClassField(cls, serialVersionUIDName, true, &einfo);
	if (fld != 0) {
		return (*(jlong*)FIELD_ADDRESS((Field*)fld));
	}

	/* Okay - since there's no field we have to compute the UID */

	/* Allocate enough uidItem space for all our needs */
	len = CLASS_NMETHODS(cls);
	if (len < CLASS_NFIELDS(cls)) {
		len = CLASS_NFIELDS(cls);
	}
	if (len < cls->interface_len) {
		len = cls->interface_len;
	}
	if (len > 0) {
		base = KMALLOC(sizeof(uidItem) * len);
	}

	SHA1Init(&c);

	/* Class -> name(UTF), modifier(INT) */
	tmp = htons(strlen(cls->name->data));
	SHA1Update(&c, (char*)&tmp, sizeof(tmp));
	/* we store the classname with slashes as path names, 
	 * but here we must use the dotted form
	 */
	classname = getClassName(cls);
	SHA1Update(&c, classname, strlen(classname));
	KFREE(classname);
	mod = htonl((int)cls->accflags & (ACC_ABSTRACT|ACC_FINAL|ACC_INTERFACE|ACC_PUBLIC));
	SHA1Update(&c, (char*)&mod, sizeof(mod));

	/* Name of each interface (sorted): UTF */

	/* Don't include the interfaces that arrays implement because Sun
	 * apparently doesn't include them either.  
	 * This is like Class.getInterfaces()
	 */
	if (!CLASS_IS_ARRAY(cls) && cls->interface_len > 0) {
		for (i = cls->interface_len-1; i >= 0; i--) {
			base[i].name = getClassName(cls->interfaces[i]);
			base[i].modifier = -1;
			base[i].desc = 0;
		}
		addToSHA(&c, base, cls->interface_len);

		/* Free all the interface name strings */
		i = cls->interface_len;
		for (i--; i >= 0; i--) {
			KFREE((char*)base[i].name);
		}
	}

	/* Each field (sorted) -> */

	i = CLASS_NFIELDS(cls);
	if (i > 0) { 
		fld = CLASS_FIELDS(cls);
		for (i--; i >= 0; i--, fld++) {
			if ((fld->accflags & ACC_PRIVATE) != 0 && ((fld->accflags & (ACC_STATIC|ACC_TRANSIENT)) != 0)) {
				base[i].name = 0;
			}
			else {
				base[i].name = fld->name->data;
				base[i].modifier = (int)fld->accflags & ACC_MASK;
				base[i].desc = getFieldDesc(fld);
			}
		}
		addToSHA(&c, base, CLASS_NFIELDS(cls));
	}

	if (CLASS_NMETHODS(cls) > 0) {

		/* Class initializer -> */

		if (findMethodLocal(cls, init_name, void_signature) != 0) {
			base[0].name = "<clinit>";
			base[0].modifier = ACC_STATIC;
			base[0].desc = "()V";
			addToSHA(&c, base, 1);
		}

		/* Each non-private constructor (sorted) -> */

		i = CLASS_NMETHODS(cls);
		mth = CLASS_METHODS(cls);
		for (i--; i >= 0; i--, mth++) {
			if ((mth->accflags & (ACC_CONSTRUCTOR|ACC_PRIVATE)) != ACC_CONSTRUCTOR) { 
				base[i].name = 0;
			}
			else {
				base[i].name = mth->name->data;
			}
			/* We do these all the time so we don't do them again */
			base[i].modifier = (int)mth->accflags & ACC_MASK;
			base[i].desc = getMethodDesc(mth);
		}
		addToSHA(&c, base, CLASS_NMETHODS(cls));

		/* Each non-private method (sorted) -> */

		i = CLASS_NMETHODS(cls);
		mth = CLASS_METHODS(cls);
		for (i--; i >= 0; i--, mth++) {
			if ((mth->accflags & (ACC_CONSTRUCTOR|ACC_PRIVATE)) != 0) { 
				base[i].name = 0;
			}
			else {
				base[i].name = mth->name->data;
			}
		}
		addToSHA(&c, base, CLASS_NMETHODS(cls));

		/* Free all the descriptor strings */
		i = CLASS_NMETHODS(cls);
		for (i--; i >= 0; i--) {
			KFREE((char*)base[i].desc);
		}
	}
	
	SHA1Final(md, &c);

	KFREE(base);

	return ( (jlong)md[0]        |
		((jlong)md[1] <<  8) |
		((jlong)md[2] << 16) |
		((jlong)md[3] << 24) |
		((jlong)md[4] << 32) |
		((jlong)md[5] << 40) |
		((jlong)md[6] << 48) |
		((jlong)md[7] << 56) );
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
			/* XXX !!! */
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
		unhand_array(array)->body[cnt] = (void*)fld;
		cnt++;
	}

	/* Sort the array */
	qsort(unhand_array(array)->body, cnt, sizeof(void*), compare);

	return (array);
}
