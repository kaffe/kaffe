/*
 * java.lang.Class.c
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
#include "../../../kaffe/kaffevm/gtypes.h"
#include "../../../kaffe/kaffevm/access.h"
#include "../../../kaffe/kaffevm/constants.h"
#include "../../../kaffe/kaffevm/object.h"
#include "../../../kaffe/kaffevm/classMethod.h"
#include "../../../kaffe/kaffevm/itypes.h"
#include "../../../kaffe/kaffevm/support.h"
#include "../../../kaffe/kaffevm/soft.h"
#include "../../../kaffe/kaffevm/stackTrace.h"
#include "InputStream.h"
#include "PrintStream.h"
#include "System.h"
#include "Constructor.h"
#include "Method.h"
#include "Field.h"
#include <native.h>
#include "defs.h"

/*
 * Convert string name to class object.
 */
struct Hjava_lang_Class*
java_lang_Class_forName(struct Hjava_lang_String* str)
{
	Hjava_lang_Class* clazz;
	Hjava_lang_Class* loadedclass;
	Hjava_lang_ClassLoader* loader;
	char buf[MAXNAMELEN];
	int depth;

	/* Get string and convert '.' to '/' */
	javaString2CString(str, buf, sizeof(buf));
	classname2pathname(buf, buf);

	/*
	 * If the calling method is in a class that was loaded by a class
	 * loader, use that class loader to find the class corresponding to
	 * the name.  Otherwise, use the system class loader.
	 */
	loadedclass = getClassWithLoader(&depth);
	loader = (depth == 0) ? loadedclass->loader : NULL;

	/*
	 * Note the following oddity: 
	 * 
	 * It is apparently perfectly legal to call forName for array types,
	 * such as "[Ljava.lang.String;" or "[B".  
	 * However, it is wrong to call Class.forName("Ljava.lang.String;")
	 *
	 * This situation is similar to the constant pool resolution.  We
	 * therefore do the same thing as in getClass in kaffevm/lookup.c,
	 * that is, use either loadArray or loadClass depending on the name.
	 *
	 * This is somewhat described in Section 5.1.3 of the VM 
	 * Specification, titled "Array Classes".  This section seems to 
	 * imply that we must avoid to ask a class loader to resolve such
	 * array names (those starting with an [), and this is what calling
	 * loadArray does.
	 */
	clazz = ((buf[0] == '[') ? loadArray : loadClass)
			(makeUtf8Const (buf, strlen(buf)), loader);

	/* if we got here, either clazz must be valid or a 
	 * ClassNotFoundException was thrown
	 */
	assert(clazz != 0);

	/*
	 * Note:
	 * The JDK documentation says: "...this method attempts to locate, 
	 * load and link the class."
	 *
	 * loadClass returns the class in state CSTATE_LINKED.
	 * Processing to CSTATE_OK will initialize the class, resolve
	 * its constants and run its static initializers.
	 *
	 * According to 2.16.4 of the VM Specification, a class should be
	 * initialized at its first active use.   The VM Specification
	 * does not mention whether Class.forName is an active use, and
	 * although 2.16.4 seems to imply that it is not, Sun's implementation 
	 * seems to run the static initializerss of a class when 
	 * Class.forName is called (see forNameTest.java)
	 *
	 * On the other hand, processing the class to CSTATE_OK is bad
	 * because our constant resolution will require that all classes
	 * to which the class refers are resolvable.  In practice, this
	 * causes ClassNotFoundExceptions to be thrown in situations where
	 * Sun would not throw such an exception, for instance, if serialver
	 * finds that .class files are missing.  This should be tolerable.
	 *
	 * Also, 2.16.4 of the VM spec says:
	 *
	 *  "The intent here is that a type has a set of initializers that 
	 *   put it in a consistent state, and that this state is the first 
	 *   state that is observed by other classes".
	 * 
	 * We are somewhat inconsistent here.  The following list lists
	 * the functions that return or process Class objects to or for
	 * the user, and the state in which the class is afterwards:
	 *
	 *	ClassLoader_defineClass0: 	returns CSTATE_PREPARED
	 * 	ClassLoader_resolveClass0:	returns CSTATE_LINKED
	 * 	ClassLoader_findSystemClass0:	returns CSTATE_LINKED
	 * 	Class_forName:			returns CSTATE_OK
	 *
	 * We have also observed that processing a class to CSTATE_OK in
	 * defineClass0 *does not* work; it causes repeated invocations of
	 * static initializers, which crashes in classMethod.c where
	 * ncode_start and ncode_end are set to zero after a initializer
	 * was invoked.
	 */
	processClass(clazz, CSTATE_OK);
	return (clazz);
}

/*
 * Convert class to string name.
 */
struct Hjava_lang_String*
java_lang_Class_getName(struct Hjava_lang_Class* c)
{
	char buffer[100];
	struct Hjava_lang_String* str;
	int len;
	char* name;
	int i;
	char* ptr;
	char ch;

	len = c->name->length;
	name = len > 100 ? (char*)malloc(len) : buffer;
	ptr = c->name->data;
	for (i = 0; i < len; i++) {
		ch = *ptr++;
		if (ch == '/') {
			ch = '.';
		}
		name[i] = ch;
	}
	str = makeJavaString(name, len);
	if (name != buffer) {
		free (name);
	}
	return (str);
}

/*
 * Create a new instance of the derived class.
 */
struct Hjava_lang_Object*
java_lang_Class_newInstance(struct Hjava_lang_Class* this)
{
	return (execute_java_constructor(0, this, "()V"));
}

/*
 * Return super class.
 */
struct Hjava_lang_Class*
java_lang_Class_getSuperclass(struct Hjava_lang_Class* this)
{
	return (this->superclass);
}

HArrayOfObject* /* [Ljava.lang.Class; */
java_lang_Class_getInterfaces(struct Hjava_lang_Class* this)
{
	HArrayOfObject* obj;
	struct Hjava_lang_Class** ifaces;
	int i;

	obj = (HArrayOfObject*)AllocObjectArray(this->interface_len, "Ljava/lang/Class");
	ifaces = (struct Hjava_lang_Class**)unhand(obj)->body;
	for (i = 0; i < this->interface_len; i++) {
		ifaces[i] = this->interfaces[i];
	}

	return (obj);
}

/*
 * Return the class loader which loaded me.
 */
struct Hjava_lang_ClassLoader*
java_lang_Class_getClassLoader(struct Hjava_lang_Class* this)
{
	return (this->loader);
}

/*
 * Is the class an interface?
 */
jbool
java_lang_Class_isInterface(struct Hjava_lang_Class* this)
{
	return ((this->accflags & ACC_INTERFACE) ? 1 : 0);
}

jbool
java_lang_Class_isPrimitive(struct Hjava_lang_Class* this)
{
	return (CLASS_IS_PRIMITIVE(this));
}

jbool
java_lang_Class_isArray(struct Hjava_lang_Class* this)
{
	return (CLASS_IS_ARRAY(this));
}

Hjava_lang_Class*
java_lang_Class_getComponentType(struct Hjava_lang_Class* this)
{
	Hjava_lang_Class* type;

	if (CLASS_IS_ARRAY(this)) {
		return (CLASS_ELEMENT_TYPE(this));
	}
	else {
		return ((Hjava_lang_Class*)0);
	}
}

jbool
java_lang_Class_isAssignableFrom(struct Hjava_lang_Class* this, struct Hjava_lang_Class* cls)
{
	return (instanceof(this, cls));
}

/*
 * Get primitive class from class name (JDK 1.1)
 */
struct Hjava_lang_Class*
java_lang_Class_getPrimitiveClass(struct Hjava_lang_String* name)
{
	jchar* chrs;

	chrs = &unhand(unhand(name)->value)->body[unhand(name)->offset];
	switch (chrs[0]) {
	case 'b':
		if (chrs[1] == 'y') {
			return (byteClass);
		}
		if (chrs[1] == 'o') {
			return (booleanClass);
		}
		break;
	case 'c':
		return (charClass);
	case 'd':
		return (doubleClass);
	case 'f':
		return (floatClass);
	case 'i':
		return (intClass);
	case 'l':
		return (longClass);
	case 's':
		return (shortClass);
	case 'v':
		return (voidClass);
	}
	return(NULL);
}

/*
 * Is object instance of this class?
 */
jbool
java_lang_Class_isInstance(struct Hjava_lang_Class* this, struct Hjava_lang_Object* obj)
{
	return (soft_instanceof(this, obj));
}

jint
java_lang_Class_getModifiers(struct Hjava_lang_Class* this)
{
	return (this->accflags & ACC_MASK);
}

HArrayOfObject*
java_lang_Class_getSigners(struct Hjava_lang_Class* this)
{
	unimp("java.lang.Class:getSigners unimplemented");
}

void
java_lang_Class_setSigners(struct Hjava_lang_Class* this, HArrayOfObject* sigs)
{
	unimp("java.lang.Class:setSigners unimplemented");
}

static
HArrayOfObject*
makeParameters(Method* meth)
{
	int i;
	int len;
	char* sig;
	HArrayOfObject* array;
	Hjava_lang_Class* clazz;

	sig = meth->signature->data;
	len = 0;
	for (sig++; *sig != ')'; sig++, len++) {
		while (*sig == '[') {
			sig++;
		}
		if (*sig == 'L') {
			while (*sig != ';') {
				sig++;
			}
		}
	}

	array = (HArrayOfObject*)AllocObjectArray(len, "Ljava/lang/Class;");
	sig = meth->signature->data + 1;	/* Skip leading '(' */
	for (i = 0; i < len; i++) {
		clazz = classFromSig(&sig, meth->class->loader);
		unhand(array)->body[i] = &clazz->head;
	}

        return (array);
}

static
Hjava_lang_Class*
makeReturn(Method* meth)
{
	char* sig;

	/* Skip to end of signature to find return type */
	sig = strchr(meth->signature->data, ')') + 1;

	return (classFromSig(&sig, meth->class->loader));
}

static
HArrayOfObject*
makeExceptions(Method* meth)
{
	/* FIXME XXX */
	return ((HArrayOfObject*)AllocObjectArray(0, "Ljava/lang/Class;"));
}

static
Hjava_lang_reflect_Constructor*
makeConstructor(struct Hjava_lang_Class* clazz, int slot)
{
	Hjava_lang_reflect_Constructor* meth;
	Method* mth;

	mth = CLASS_METHODS(clazz) + slot;
	meth = (Hjava_lang_reflect_Constructor*)AllocObject("java/lang/reflect/Constructor");      

	unhand(meth)->clazz = clazz;
	unhand(meth)->slot = slot;
	unhand(meth)->parameterTypes = makeParameters(mth);
	unhand(meth)->exceptionTypes = makeExceptions(mth);

	return (meth);
}

static
Hjava_lang_reflect_Method*
makeMethod(struct Hjava_lang_Class* clazz, int slot)
{
	Hjava_lang_reflect_Method* meth;
	Method* mth;

	mth = CLASS_METHODS(clazz) + slot;
	meth = (Hjava_lang_reflect_Method*)AllocObject("java/lang/reflect/Method");      

	unhand(meth)->clazz = clazz;
	unhand(meth)->slot = slot;
	unhand(meth)->name = makeReplaceJavaStringFromUtf8(
		mth->name->data, mth->name->length, 0, 0);
	unhand(meth)->parameterTypes = makeParameters(mth);
	unhand(meth)->exceptionTypes = makeExceptions(mth);
	unhand(meth)->returnType = makeReturn(mth);

	return (meth);
}

static
Hjava_lang_reflect_Field*
makeField(struct Hjava_lang_Class* clazz, int slot)
{
	Hjava_lang_reflect_Field* field;
	Field* fld;

	fld = CLASS_FIELDS(clazz) + slot;
	field = (Hjava_lang_reflect_Field*)AllocObject("java/lang/reflect/Field");
	unhand(field)->clazz = clazz;
	unhand(field)->slot = slot;
	unhand(field)->type = resolveFieldType(fld, clazz);
	unhand(field)->name = makeReplaceJavaStringFromUtf8(
		fld->name->data, fld->name->length, 0, 0);
	return (field);
}
 
HArrayOfObject*
java_lang_Class_getMethods0(struct Hjava_lang_Class* this, jint declared)
{
	int count;
	Hjava_lang_Class* clas;
	Method* mth;
	Hjava_lang_reflect_Method** ptr;
	HArrayOfObject* array;
	int i;

	count = 0;
	for (clas = this; clas != NULL; clas = clas->superclass) {
		mth = CLASS_METHODS(clas);
		i = CLASS_NMETHODS(clas);
		for (i = CLASS_NMETHODS(clas)-1 ; i >= 0; i--) {
			if (((mth[i].accflags & ACC_PUBLIC) || declared) && !(mth[i].accflags & ACC_CONSTRUCTOR)) {
				count++;
			}
		}
		if (declared) {
			break;
		}
	}
	array = (HArrayOfObject*)AllocObjectArray(count, "Ljava/lang/reflect/Method;");
	ptr = (Hjava_lang_reflect_Method**)&unhand(array)->body[0];
	for (clas = this; clas != NULL; clas = clas->superclass) {
		mth = CLASS_METHODS(clas);
		for (i = CLASS_NMETHODS(clas)-1; i >= 0; i--) {
			if (((mth[i].accflags & ACC_PUBLIC) || declared) && !(mth[i].accflags & ACC_CONSTRUCTOR)) {
				*ptr = makeMethod(clas, i);
				ptr++;
			}
		}
		/* End if looking at declared */
		if (declared) {
			break;
		}
	}

	return (array);
}

HArrayOfObject*
java_lang_Class_getConstructors0(struct Hjava_lang_Class* this, jint declared)
{
	int count;
	Hjava_lang_Class* clas;
	Method* mth;
	Hjava_lang_reflect_Constructor** ptr;
	HArrayOfObject* array;
	int i;

	count = 0;
	clas = this;
	mth = CLASS_METHODS(clas);
	for (i = CLASS_NMETHODS(clas)-1; i >= 0;  i--) {
		if (((mth[i].accflags & ACC_PUBLIC) || declared) && (mth[i].accflags & ACC_CONSTRUCTOR)) {
			count++;
		}
	}
	array = (HArrayOfObject*)AllocObjectArray(count, "Ljava/lang/reflect/Constructor;");
	ptr = (Hjava_lang_reflect_Constructor**)&unhand(array)->body[0];
	clas = this;
	mth = CLASS_METHODS(clas);
	for (i = CLASS_NMETHODS(clas)-1; i >= 0;  i--) {
		if (((mth[i].accflags & ACC_PUBLIC) || declared) && (mth[i].accflags & ACC_CONSTRUCTOR)) {
			*ptr = makeConstructor(clas, i);
			ptr++;
		}
	}
	return (array);
}

HArrayOfObject*
java_lang_Class_getFields0(struct Hjava_lang_Class* clazz, int declared)
{
	int count;
	Hjava_lang_Class* clas;
	Field* fld;
	Hjava_lang_reflect_Field** ptr;
	HArrayOfObject* array;
	int i;

	if (declared) {
		count = CLASS_NFIELDS((Hjava_lang_Class*)clazz);
	}
	else {
		count = 0;
		for (clas = clazz; clas != NULL; clas = clas->superclass) {
			fld = CLASS_FIELDS(clas);
			for (i = CLASS_NFIELDS(clas)-1; i >= 0; i--) {
				if (fld[i].accflags & ACC_PUBLIC) {
					count++;
				}
			}
		}
	}
	array = (HArrayOfObject*)AllocObjectArray(count, "Ljava/lang/reflect/Field;");
	ptr = (Hjava_lang_reflect_Field**)&unhand(array)->body[0];
	for (clas = clazz; clas != NULL; clas = clas->superclass) {
		fld = CLASS_FIELDS(clas);
		for (i = CLASS_NFIELDS(clas)-1; i >= 0;  i--) {
			if ((fld[i].accflags & ACC_PUBLIC) || declared) {
				*ptr = makeField(clas, i);
				ptr++;
			}
		}
		if (declared) {
			break;
		}
	}

	return (array);
}

/*
 * Check whether the parameters of Method `mth' have exactly the same
 * types as `argtypes', where argtypes is an array of Hjava_lang_Class *
 *
 * Note that checking the arguments might cause the resolution of names
 * that are part of the signature.  These must be resolved by the same 
 * classloader that loaded the class to which the method belongs.
 *
 * This function is used by getMethod0 and getConstructor0.
 *
 * Returns 1 if they are exactly the same, 0 if not.
 */
static
int
checkParameters(Method* mth, HArrayOfObject* argtypes)
{
	char *sig = mth->signature->data;
	int   i;

	sig++;	/* skip leading '(' */
	for (i = 0; i < ARRAY_SIZE(argtypes); i++) {

		/* signature was too short or type doesn't match */
		if (!*sig || (struct Hjava_lang_Class *)
				OBJARRAY_DATA(argtypes)[i] != 
				classFromSig(&sig, mth->class->loader))
			return (0);
	}
	/* return false if signature was too long */
	return (*sig == ')' ? 1 : 0);
}

Hjava_lang_reflect_Method*
java_lang_Class_getMethod0(struct Hjava_lang_Class* this, struct Hjava_lang_String* name, HArrayOfObject* arr, jint declared)
{
	Hjava_lang_Class* clas;

	clas = this;
	do {
		Method* mth = CLASS_METHODS(clas);
		int n = CLASS_NMETHODS(clas);
		int i;
		for (i = 0;  i < n;  ++mth, ++i) {
			if (((mth->accflags & ACC_PUBLIC) || declared)
			    && equalUtf8JavaStrings (mth->name, name)) {
				if (checkParameters(mth, arr))
					return (makeMethod(clas, i));
			}
		}
		clas = clas->superclass;
	} while (!declared && clas != NULL);

	/* like SignalError, except that the name of the class that is
	 * not found becomes the error message 
	 */
	throwException(execute_java_constructor(
		"java.lang.NoSuchMethodException", 0, 
		"(Ljava/lang/String;)V", name));
}

struct Hjava_lang_reflect_Constructor*
java_lang_Class_getConstructor0(struct Hjava_lang_Class* this, HArrayOfObject* arr, jint declared) 
{
	Hjava_lang_Class* clas = this;

	Method* mth = CLASS_METHODS(clas);
	int n = CLASS_NMETHODS(clas);
	int i;
	for (i = 0;  i < n;  ++mth, ++i) {
		if (((mth->accflags & ACC_PUBLIC) || declared) && (mth->accflags & ACC_CONSTRUCTOR)) {
			if (checkParameters(mth, arr))
				return (makeConstructor(clas, i));
		}
	}

	SignalError("java.lang.NoSuchMethodException", ""); /* FIXME */
}

Hjava_lang_reflect_Field*
java_lang_Class_getField0(struct Hjava_lang_Class* clazz, struct Hjava_lang_String* name, int declared)
{
	Hjava_lang_Class* clas;

	clas = (Hjava_lang_Class*) clazz;
	do {
		Field* fld = CLASS_FIELDS(clas);
		int n = CLASS_NFIELDS(clas);
		int i;
		for (i = 0;  i < n;  ++fld, ++i) {
			if (((fld->accflags & ACC_PUBLIC) || declared)
			    && equalUtf8JavaStrings (fld->name, name)) {
				return makeField(clas, i);
			}
		}
		clas = clas->superclass;
	} while (!declared && clas != NULL);
	SignalError("java.lang.NoSuchFieldException", ""); /* FIXME */
}
