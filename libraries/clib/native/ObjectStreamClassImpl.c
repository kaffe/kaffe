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
#include "../../../kaffe/kaffevm/itypes.h"
#include "../../../kaffe/kaffevm/lookup.h"
#include "../../../kaffe/kaffevm/access.h"
#include "../../../kaffe/kaffevm/stringSupport.h"
#include "../../../kaffe/kaffevm/support.h"
#include "../../../kaffe/kaffevm/exception.h"
#include "../../../kaffe/kaffevm/baseClasses.h"
#include "../../../kaffe/kaffevm/sha-1.h"
#include <native.h>
#include "java_io_ObjectInputStream.h"
#include "java_io_ObjectOutputStream.h"
#include "java_io_ObjectStreamClass.h"
#include "java_io_ObjectStreamField.h"

#include <jni.h>

/*
 * Technically, this file should probably be named ObjectStreamClass.c
 * (without the Impl).
 */
static char* getClassName(Hjava_lang_Class* cls);

/* NB: these guys are all write once and then immutable */
static Utf8Const* serialVersionUIDName;
static Utf8Const* writeObjectName;
static Utf8Const* readObjectName;
static Utf8Const* ObjectOutputStreamSig;
static Utf8Const* ObjectInputStreamSig;
static Utf8Const* serialPersistentFieldsName;
static Hjava_lang_Class* ObjectStreamFieldClass;
static Hjava_lang_Class* ObjectStreamFieldClassArray;

/*
 * Used to hold a descriptor item while calculating the serialUID.
 */
typedef struct {
	const char*	name;
	const char*	desc;
	int		modifier;
} uidItem;


void
java_io_ObjectStreamClass_init0(void)
{
	errorInfo einfo;

	serialVersionUIDName = checkPtr(utf8ConstNew("serialVersionUID" , -1));
	writeObjectName = utf8ConstNew("writeObject", -1);
	if (!writeObjectName) {
		postOutOfMemory(&einfo);
		goto svun;
	}
	readObjectName = utf8ConstNew("readObject", -1);
	if (!readObjectName) {
		postOutOfMemory(&einfo);
		goto won;
	}
	ObjectOutputStreamSig = utf8ConstNew("(Ljava/io/ObjectOutputStream;)V", -1);
	if (!ObjectOutputStreamSig) {
		postOutOfMemory(&einfo);
		goto ron;
	}
	ObjectInputStreamSig = utf8ConstNew("(Ljava/io/ObjectInputStream;)V", -1);
	if (!ObjectInputStreamSig) {
		postOutOfMemory(&einfo);
		goto oos;
	}
	serialPersistentFieldsName = utf8ConstNew("serialPersistentFields", -1);
	if (!serialPersistentFieldsName) {
		postOutOfMemory(&einfo);
		goto spfn;
	}
	ObjectStreamFieldClass = lookupClass("java/io/ObjectStreamField", NULL, &einfo);
	ObjectStreamFieldClassArray = lookupClass("[Ljava/io/ObjectStreamField;", NULL, &einfo);
	if ((ObjectStreamFieldClass == 0)
	    || (ObjectStreamFieldClassArray == 0)) {
		utf8ConstRelease(serialPersistentFieldsName);
spfn:		utf8ConstRelease(ObjectInputStreamSig);
oos: 		utf8ConstRelease(ObjectOutputStreamSig);
ron: 		utf8ConstRelease(readObjectName);
won: 		utf8ConstRelease(writeObjectName);
svun: 		utf8ConstRelease(serialVersionUIDName);
		throwError(&einfo);
	}
}

struct Hjava_lang_Object*
java_io_ObjectStreamClass_allocateNewObject(struct Hjava_io_ObjectStreamClass* cls)
{
	Hjava_lang_Object* obj;
	Hjava_lang_Class* clazz;
	Method* meth;

	/* Get the class and create an empty object of that type */
	clazz = unhand(cls)->clazz;
	obj = newObject(clazz);

	/* Work our way down the superstreams until we get to the point where
	 * we no longer are serializing.  We get the superclass at that point
	 * and invoke the <init>() constructor.
	 */
	do {
		clazz = clazz->superclass;
		cls = unhand(cls)->superstream;
	} while (cls != 0);

	meth = findMethodLocal(clazz, constructor_name, void_signature);
	if (meth == 0) {
		SignalErrorf("java.io.InvalidClassException",
			     "%s; Missing no-arg constructor for class",
			     CLASS_CNAME(clazz));
		return (0);
	}
	else if ((meth->accflags & (ACC_CONSTRUCTOR|ACC_PRIVATE)) != ACC_CONSTRUCTOR) {
		SignalErrorf("java.io.InvalidClassException",
			     "%s; IllegalAccessException",
			     CLASS_CNAME(clazz));
		return (0);
	}
	else {
		do_execute_java_method(obj, 0, 0, meth, 0);
		return (obj);
	}
}

struct Hjava_lang_Object*
java_io_ObjectStreamClass_allocateNewArray(struct Hjava_io_ObjectStreamClass* cls, jint sz)
{
	return (newArray(CLASS_ELEMENT_TYPE(unhand(cls)->clazz), sz));
}

/* Invoked by ObjectStreamClass.defaultReadObject()
 *
 * private native static void inputClassFields0(
 * 		Class clazz, ObjectStreamField[] readFields, 
 * 		Object obj, ObjectInputStream in);
 */
void
java_io_ObjectStreamClass_inputClassFields0(Hjava_lang_Class* clazz,
					    HArrayOfObject* readFields, /* ObjectStreamField[] readFields */
					    struct Hjava_lang_Object* obj,
					    struct Hjava_io_ObjectInputStream* in)
{
	int i;
	int len;
	Hjava_io_ObjectStreamField** fld;

	assert(clazz != 0); /* XXX just used for pretty exception messages... */
	assert(readFields != 0);
	assert(obj != 0);
	assert(in != 0);

	fld = (Hjava_io_ObjectStreamField**)unhand_array(readFields)->body;
	len = obj_length(readFields);

	for (i = 0; i < len; i++, fld++) {
		char typeCode;

		if (unhand(*fld)->typeMismatch)
		{
			/*
			 * If there was a typeMismatch, we discovered
			 * that by comparing the *names* of the types
			 * in the inStream repr and the inVM repr.
			 * Thus, the 'type' field of *fld is null, and
			 * can't be used in the error message.  We
			 * could use (*fld)->typeName, but that's a
			 * java.lang.String and is surprisingly
			 * annoying to stick in an exception message.
			 */
			char fldname[128];
			stringJava2CBuf(unhand(*fld)->name, fldname, sizeof(fldname));
			SignalErrorf("java.io.InvalidClassException",
				     "Field `%s' incompatible serialization type in %s.",
				     fldname,
				     CLASS_CNAME(clazz));
		}

/*
 * Run the right reader method on 'in', and store result at right
 * offset into 'obj'.  Silently ignore fields that don't exist in the
 * class.
 */
#define SETOBJFIELD(FUNC,SIG,TYPE)						\
	if (unhand(*fld)->offset == -1)						\
		do_execute_java_method(in, FUNC, SIG, 0, 0);			\
	else									\
		((jvalue*)(((uint8*)obj) + unhand(*fld)->offset))->TYPE =	\
			do_execute_java_method(in, FUNC, SIG, 0, 0).TYPE

		typeCode = unhand(*fld)->typeCode;

		assert(typeCode > 0);
		assert(typeCode < 127); /* Should be valid ASCII char... */

		if (typeCode == 'L' || typeCode == '[') {
                        SETOBJFIELD("readObject", "()Ljava/lang/Object;", l);
		}
		else switch (typeCode) {
                case 'B':
                        SETOBJFIELD("readByte", "()B", b);
                        break;
                case 'C':
                        SETOBJFIELD("readChar", "()C", c);
                        break;
                case 'D':
                        SETOBJFIELD("readDouble", "()D", d);
                        break;
                case 'F':
                        SETOBJFIELD("readFloat", "()F", f);
                        break;
                case 'J':
                        SETOBJFIELD("readLong", "()J", j);
                        break;
                case 'S':
                        SETOBJFIELD("readShort", "()S", s);
                        break;
                case 'Z':
                        SETOBJFIELD("readBoolean", "()Z", z);
                        break;
                case 'I':
                        SETOBJFIELD("readInt", "()I", i);
                        break;
                default:
                        SignalErrorf("java.io.InvalidClassException",
				     "Unknown data type %s in class %s",
				     CLASS_CNAME(unhand(*fld)->type),
				     CLASS_CNAME(clazz));
		}
	}
}

/*
 * private static native void outputClassFields0(Class clazz,
 *						 ObjectStreamField[] writableFields,
 *						 Object obj,
 *						 ObjectOutputStream out);
 */
void
java_io_ObjectStreamClass_outputClassFields0(Hjava_lang_Class* clazz,
					     HArrayOfObject* serializableFields, 
					     Hjava_lang_Object* obj,
					     Hjava_io_ObjectOutputStream* out)
{
	int i;
	int len;
	Hjava_io_ObjectStreamField** fld;

	assert(clazz != 0); /* clazz just used for pretty error messages. */
	assert(serializableFields != 0);
	assert(obj != 0);
	assert(out != 0);

	fld = (Hjava_io_ObjectStreamField**)unhand_array(serializableFields)->body;
	len = obj_length(serializableFields);

/* Write field of 'obj' to 'out'. */
#define WRITE(FUNC,SIG,TYPE) \
	do_execute_java_method(out, FUNC, SIG, 0, 0, \
		((jvalue*)(((uint8*)obj) + unhand(*fld)->offset))->TYPE)

	for (i = 0; i < len; i++, fld++) {
		if (unhand(*fld)->offset == -1)
		{
			char fldname[128];
			stringJava2CBuf(unhand(*fld)->name, fldname, sizeof(fldname));
			SignalErrorf("java.io.InvalidClassException",
				     "No field named `%s' in type %s.",
				     fldname, 
				     CLASS_CNAME(clazz));
		}

		if (unhand(*fld)->typeMismatch)
		{
			char fldname[128];
			stringJava2CBuf(unhand(*fld)->name, fldname, sizeof(fldname));
			SignalErrorf("java.io.InvalidClassException",
				     "Field `%s' serialization type (%s) does not match actual type in %s.",
				     fldname,
				     CLASS_CNAME(unhand(*fld)->type),
				     CLASS_CNAME(clazz));
		}


		if (! CLASS_IS_PRIMITIVE(unhand(*fld)->type)) {
                        WRITE("writeObject", "(Ljava/lang/Object;)V", l);
		}
		else switch (CLASS_PRIM_SIG(unhand(*fld)->type)) {
                case 'B':
                        WRITE("writeByte", "(I)V", b);
                        break;
                case 'C':
                        WRITE("writeChar", "(I)V", c);
                        break;
                case 'D':
                        WRITE("writeDouble", "(D)V", d);
                        break;
                case 'F':
                        WRITE("writeFloat", "(F)V", f);
                        break;
                case 'J':
                        WRITE("writeLong", "(J)V", j);
                        break;
                case 'S':
                        WRITE("writeShort", "(I)V", s);
                        break;
                case 'Z':
                        WRITE("writeBoolean", "(Z)V", z);
                        break;
                case 'I':
                        WRITE("writeInt", "(I)V", i);
                        break;
                default:
                        SignalError("java.io.InvalidClassException", "Unknown data type");
		}
	}
}

jboolean
java_io_ObjectStreamClass_invokeObjectReader0(struct Hjava_io_ObjectStreamClass* cls,
					      struct Hjava_lang_Object* obj,
					      struct Hjava_io_ObjectInputStream* in)
{
	Method* meth;
	Hjava_lang_Object* oldObj;
	struct Hjava_io_ObjectStreamClass* oldCls;

	assert(cls);
	assert(obj);
	assert(in);

	oldObj = unhand(in)->currentObject;
	oldCls = unhand(in)->currentStreamClass;
	unhand(in)->currentObject = obj;
	unhand(in)->currentStreamClass = (struct Hjava_io_ObjectStreamClass*)cls;

	meth = findMethodLocal(unhand(cls)->clazz, readObjectName, ObjectInputStreamSig);
	if (meth == 0) {
		errorInfo info;
		postExceptionMessage(&info, JAVA_LANG(NoSuchMethodError), readObjectName->data);
		throwError(&info);
	}
	do_execute_java_method(obj, 0, 0, meth, 0, in);

	unhand(in)->currentObject = oldObj;
	unhand(in)->currentStreamClass = oldCls;

	return (true);
}

jboolean
java_io_ObjectStreamClass_invokeObjectWriter0(struct Hjava_io_ObjectStreamClass* cls, struct Hjava_lang_Object* obj, struct Hjava_io_ObjectOutputStream* out)
{
	Method* meth;
	Hjava_lang_Object* oldObj;
	struct Hjava_io_ObjectStreamClass* oldCls;

	oldObj = unhand(out)->currentObject;
	oldCls = unhand(out)->currentStreamClass;
	unhand(out)->currentObject = obj;
	unhand(out)->currentStreamClass = (struct Hjava_io_ObjectStreamClass*)cls;

	meth = findMethodLocal(unhand(cls)->clazz, writeObjectName, ObjectOutputStreamSig);
	if (meth == 0) {
		errorInfo info;
		postExceptionMessage(&info, JAVA_LANG(NoSuchMethodError), writeObjectName->data);
		throwError(&info);
	}
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

	/* push null entries to the right end of the array */
	if (o->name == 0) {
		return (1);
	}
	if (t->name == 0) {
		return (-1);
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
 * Take a class name in slashed form pkg/subpkg/name, returns 
 * a newly allocated one in dot form pkg.subpkg.name.
 * Caller must free using KFREE.
 */
char*
pathname2ClassnameCopy(const char *orig)
{
	char* str;
	str = KMALLOC(strlen(orig) + 1);
	if (str != 0) {
		pathname2classname(orig, str);
	}
	return (str);
}

/* Return field description in slashed form, like Ljava/lang/String;
 * or like [I or like C or like [[Ljava/lang/String;
 * Caller frees with KFREE.
 */
static
const char*
getFieldDesc(Field* fld)
{
	char* str;
	const char* orig;

	if (!FIELD_RESOLVED(fld)) {
		/* This is like so: Ljava/lang/String; */
		orig = ((Utf8Const*)(void*)fld->type)->data;
		str = KMALLOC(strlen(orig) + 1);
		return (str ? strcpy(str, orig) : 0);
	}
	else if (!CLASS_IS_PRIMITIVE(FIELD_TYPE(fld))) {
		/* This is like so: java.lang.String */
		orig = FIELD_TYPE(fld)->name->data;
		if (orig[0] == '[') {
			/* arrays should be fine */
			str = KMALLOC(strlen(orig) + 1);
			if (str) classname2pathname(orig, str);
			return (str);
		} else {
			str = KMALLOC(strlen(orig) + 3);
			if (!str) return 0;
			strcpy(str, "L");
			strcat(str, orig);
			strcat(str, ";");
			classname2pathname(str, str);
			return (str);
		}
	}
	else {
		orig = CLASS_PRIM_NAME(FIELD_TYPE(fld))->data;
		str = KMALLOC(strlen(orig) + 1);
		return (str ? strcpy(str, orig) : 0);
	}
}

static
char*
getMethodDesc(Method* mth)
{
	return (pathname2ClassnameCopy(METHOD_SIGD(mth)));
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
java_io_ObjectStreamClass_getSerialVersionUID0(Hjava_lang_Class* cls)
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

	/* For GCJ compatibility, we cannot hardcode this in java/lang/Class 
	 * This number won't change as long as the universe exists, so it's
	 * safe to keep it here.
	 */
	if (cls == ClassClass) {
		return (3206093459760846163LL);
	}

	/* Lookup _local_ field serialVersionUID */
	for (fld = CLASS_SFIELDS(cls), i = CLASS_NSFIELDS(cls); i-- > 0; fld++) {
		if (utf8ConstEqual (serialVersionUIDName, fld->name) &&
		    (fld->accflags & (ACC_STATIC|ACC_FINAL)) == (ACC_STATIC|ACC_FINAL)) {
			Hjava_lang_Class* ftype;

			/* XXX if ACC_PUBLIC, throw error */

			ftype = resolveFieldType(fld, cls, &einfo);
			if (ftype == 0) {
				throwError(&einfo);
			}

			/* See JDC Bug 4431318:  The serialization spec does
			 * not state what should occur if serialVersionUID is
			 * declared to be of a type other than long; throwing
			 * an IllegalArgumentException, while not particularly
			 * graceful, is not strictly in violation of the spec.
			 * A reasonable solution may be for serialization to
			 * throw InvalidClassExceptions in cases like this.
			 *
			 * Use JDK1.1 behavior until serialization spec states what
			 * should occur.  */
			if (ftype == longClass) {
				return (*(jlong*)FIELD_ADDRESS(fld));
			}
			else {
				break;
			}
		}
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
		if (!base) {
			postOutOfMemory(&einfo);
			throwError(&einfo);
		}
	}

	SHA1Init(&c);

	/* Class -> name(UTF), modifier(INT) */
	/* we store the classname with slashes as path names, 
	 * but here we must use the dotted form
	 */
	classname = getClassName(cls);
	if (!classname) {
		KFREE(base);
		postOutOfMemory(&einfo);
		throwError(&einfo);
	}
	tmp = htons(strlen(classname));
	SHA1Update(&c, (char*)&tmp, sizeof(tmp));
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
			if (!base[i].name) {
				int j = cls->interface_len;

				for (j--; j > i; j--)
					KFREE(base[j].name);
				KFREE(base);
				postOutOfMemory(&einfo);
				throwError(&einfo);
			}
		}
		addToSHA(&c, base, cls->interface_len);

		/* Free all the interface name strings */
		i = cls->interface_len;
		for (i--; i >= 0; i--) {
			KFREE(base[i].name);
		}
	}

	/* Each field (sorted) -> */

	i = CLASS_NFIELDS(cls);
	if (i > 0) { 
		fld = CLASS_FIELDS(cls);
		for (i--; i >= 0; i--, fld++) {
			if ((fld->accflags & ACC_PRIVATE) != 0 && ((fld->accflags & (ACC_STATIC|ACC_TRANSIENT)) != 0)) {
				base[i].name = 0;
				base[i].desc = 0;
			}
			else {
				base[i].name = fld->name->data;
				base[i].modifier = (int)fld->accflags & ACC_MASK;
				base[i].desc = getFieldDesc(fld);
				if (!base[i].desc) {
					int j = CLASS_NFIELDS(cls);

					for (j--; j > i; j--)
						KFREE(base[j].desc);
					KFREE(base);
					postOutOfMemory(&einfo);
					throwError(&einfo);
				}
			}
		}
		addToSHA(&c, base, CLASS_NFIELDS(cls));

		/* free descriptors */
		i = CLASS_NFIELDS(cls);
		for (i--; i >= 0; i--) {
			KFREE(base[i].desc);
		}
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
			base[i].modifier = (int)mth->accflags & ACC_MASK;
			base[i].desc = getMethodDesc(mth);
			if (!base[i].desc) {
				int j = CLASS_NMETHODS(cls);

				for (j--; j > i; j--)
					KFREE(base[j].desc);
				KFREE(base);
				postOutOfMemory(&einfo);
				throwError(&einfo);
			}
		}
		addToSHA(&c, base, CLASS_NMETHODS(cls));

		/* Free all the descriptor strings */
		i = CLASS_NMETHODS(cls);
		for (i--; i >= 0; i--) {
			KFREE(base[i].desc);
		}

		/* NB: we can't reuse the base array here because
		 * addToSHA has qsorted it.
		 */

		/* Each non-private method (sorted) -> */

		i = CLASS_NMETHODS(cls);
		mth = CLASS_METHODS(cls);
		for (i--; i >= 0; i--, mth++) {
			/* skip private methods, constructors, and
			 * <clinit> again.  Do not include <clinit> twice 
			 */
			if (((mth->accflags & (ACC_CONSTRUCTOR|ACC_PRIVATE)) != 0) || (utf8ConstEqual(mth->name, init_name) && utf8ConstEqual(METHOD_SIG(mth), void_signature))) { 
				base[i].name = 0;
			}
			else {
				base[i].name = mth->name->data;
			}
			base[i].modifier = (int)mth->accflags & ACC_MASK;
			base[i].desc = getMethodDesc(mth);
			if (!base[i].desc) {
				int j = CLASS_NMETHODS(cls);

				for (j--; j > i; j--)
					KFREE(base[j].desc);
				KFREE(base);
				postOutOfMemory(&einfo);
				throwError(&einfo);
			}
		}
		addToSHA(&c, base, CLASS_NMETHODS(cls));

		/* Free all the descriptor strings */
		i = CLASS_NMETHODS(cls);
		for (i--; i >= 0; i--) {
			KFREE(base[i].desc);
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
java_io_ObjectStreamClass_hasWriteObject0(struct Hjava_lang_Class* clazz)
{
	if (findMethodLocal(clazz, writeObjectName, ObjectOutputStreamSig) == 0) {
		return (false);
	}
	else {
		return (true);
	}
}

/* Find field in the given class with the given name.  Returns 0 if not found. */
static Field*
findField(Hjava_lang_Class* clazz, Hjava_lang_String* jFieldName)
{
	Field* fld;
	int i;

	for (fld = CLASS_IFIELDS(clazz), i = CLASS_NIFIELDS(clazz);
	     i-- > 0;
	     fld++)
	{
		if (utf8ConstEqualJavaString(fld->name, jFieldName))
			return fld;
	}

	return 0;
}

/* Can throw exceptions. */
static HArrayOfObject*
deepCopyFixup(Hjava_lang_Class* clazz, HArrayOfObject* origArray)
{
	const int len = obj_length(origArray);
	HArrayOfObject* nArray;
	int i;

	nArray = (HArrayOfObject*)newArray(ObjectStreamFieldClass, len);
	for (i = 0; i < len; i++)
	{
		const Hjava_io_ObjectStreamField* origField;
		Hjava_io_ObjectStreamField* newField;
		Field* fld;

		origField = (Hjava_io_ObjectStreamField*)(unhand_array(origArray)->body[i]);
		newField = (Hjava_io_ObjectStreamField*)newObject(ObjectStreamFieldClass);

		assert(unhand(origField)->typeCode > 0);

		unhand(newField)->type = unhand(origField)->type;
		unhand(newField)->typeCode = unhand(origField)->typeCode;
		unhand(newField)->name = unhand(origField)->name;
		unhand(newField)->unshared = unhand(origField)->unshared;

		fld = findField(clazz, unhand(origField)->name);

		if (fld == 0)
		{
			unhand(newField)->offset = -1;
			unhand(newField)->typeMismatch = 0;
		}
		else
		{
			errorInfo einfo;

			unhand(newField)->offset = FIELD_BOFFSET(fld);

			if (!resolveFieldType(fld, clazz, &einfo))
				throwError(&einfo);

			/* Basically, we cache the type mismatch so we don't have to re-discover it later. */
			if (FIELD_TYPE(fld) != unhand(newField)->type)
				unhand(newField)->typeMismatch = 1;
			else
				unhand(newField)->typeMismatch = 0;
		}

		unhand_array(nArray)->body[i] = (void*)newField;
	}

	return nArray;
}


/* XXX redundant with ObjectStreamField.compareTo() */
static int
compare(const void* o, const void* t)
{
	Hjava_io_ObjectStreamField* one = *(Hjava_io_ObjectStreamField**)o;
	Hjava_io_ObjectStreamField* two = *(Hjava_io_ObjectStreamField**)t;

	bool oneIsRef = !CLASS_IS_PRIMITIVE(unhand(one)->type);
	bool twoIsRef = !CLASS_IS_PRIMITIVE(unhand(two)->type);

	/*
	 * References beat primitives.  Name is secondary sort.
	 */

	if (oneIsRef) { 
		if (!twoIsRef) {
			return (1); /* first is "greater" */
		}
	}
	else if (twoIsRef) {
		return (-1); /* second is "greater" */
	}

	/* XXX String.compareTo(String s).  Move into string.c? */
	{
		Hjava_lang_String* name1 = unhand(one)->name;
		Hjava_lang_String* name2 = unhand(two)->name;
		int len1 = STRING_SIZE(name1);
		int len2 = STRING_SIZE(name2);
		int min;
		int pos;

		min = len1;
		if (len2 < min)
			min = len2;
		for (pos = 0; pos < min; pos++)
		{
			jchar c1 = STRING_DATA(name1)[pos];
			jchar c2 = STRING_DATA(name2)[pos];
			if (c1 != c2)
				return c1 - c2;
		}

		return (len1 - len2);
	}
}

// private static native ObjectStreamField[] findSerializableFields0(Class clazz);
HArrayOfObject* /*Array of Hjava_io_ObjectStreamField* */
java_io_ObjectStreamClass_findSerializableFields0(struct Hjava_lang_Class* clazz)
{
	errorInfo einfo;
	int len;
	Field* fld;
	HArrayOfObject* array;
	int i;
	int cnt;

	/*
	 * Make sure the static initializers of clazz have run
	 */
	if (!processClass(clazz, CSTATE_COMPLETE, &einfo)) {
		throwError(&einfo);
	}

	/*
	 * Lookup _local_ field "serialPersistentFields". If it exists,
	 * make a deep copy and return the deep copy.  Fixup the deeply
	 * copied ObjectStreamField objects to have the correct
	 * offset (-1 if the field doesn't exist in the class).
	 */
	for (fld = CLASS_SFIELDS(clazz), i = CLASS_NSFIELDS(clazz);
	     i-- > 0;
	     fld++)
	{
		if (utf8ConstEqual (serialPersistentFieldsName, fld->name) &&
		    /* Note, field is silently ignored if not private. */
		    ((fld->accflags & (ACC_PRIVATE|ACC_STATIC|ACC_FINAL))
		     == (ACC_PRIVATE|ACC_STATIC|ACC_FINAL))) {
			HArrayOfObject* userArray;
			Hjava_lang_Class* ftype;

			ftype = resolveFieldType(fld, clazz, &einfo);
			if (ftype == 0) {
				throwError(&einfo);
			}

			/* Silently ignore if its the wrong type. */
			if (ftype != ObjectStreamFieldClassArray)
				break;
			
			userArray = *((HArrayOfObject**)FIELD_ADDRESS(fld));

			/*
			 * XXX the JDK says (see bug 4334265) that a null
			 * serialPersistentFields is simply ignored (it
			 * really should be equivalent to an empty array...)
			 */
			if (userArray == 0)
				break;

			array = deepCopyFixup(clazz, userArray);

			/* Fields must be in canonical order. */
			qsort(unhand_array(array)->body, obj_length(array), sizeof(void*), compare);
			return array;
		}
	}

	/*
	 * Create a custom ObjectStreamField array.  One entry for
	 * each non-transient instance field.  (We skip silly
	 * inner-class crap, too.)
	 */

	/* Count the number of fields we need to store */
	len = CLASS_NIFIELDS(clazz);
	fld = CLASS_IFIELDS(clazz);
	cnt = 0;
	for (i = 0; i < len; i++, fld++) {
		if ((fld->accflags & ACC_TRANSIENT) != 0) {
			continue;
		}
		/* Skip innerclass nonsense */
		if (strncmp(fld->name->data, "this$", 5) == 0) {
			continue;
		}
		cnt++;
	}

	/* Build an array of those fields */
	array = (HArrayOfObject*)newArray(ObjectStreamFieldClass, cnt);
	cnt = 0;
	fld = CLASS_IFIELDS(clazz);
	for (i = 0; i < len; i++, fld++) {
		Hjava_io_ObjectStreamField* newField;

		if ((fld->accflags & ACC_TRANSIENT) != 0) {
			continue;
		}
		/* Skip innerclass nonsense */
		if (strncmp(fld->name->data, "this$", 5) == 0) {
			continue;
		}

		newField = (Hjava_io_ObjectStreamField*)newObject(ObjectStreamFieldClass);
		unhand(newField)->type = resolveFieldType(fld, clazz, &einfo);
		if (unhand(newField)->type == 0)
			throwError(&einfo);
		if (CLASS_IS_PRIMITIVE(unhand(newField)->type))
			unhand(newField)->typeCode = CLASS_PRIM_SIG(unhand(newField)->type);
		else if (CLASS_IS_ARRAY(unhand(newField)->type))
			unhand(newField)->typeCode = '[';
		else
			unhand(newField)->typeCode = 'L';
		unhand(newField)->name = utf8Const2Java(fld->name);
		unhand(newField)->unshared = 0;
		unhand(newField)->offset = FIELD_BOFFSET(fld);
		unhand(newField)->typeMismatch = false;

		unhand_array(array)->body[cnt] = (void*)newField;
		cnt++;
	}

	/* Fields must be in canonical order. */
	qsort(unhand_array(array)->body, cnt, sizeof(void*), compare);

	return (array);
}
