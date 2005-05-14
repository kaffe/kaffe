/*
 * java.lang.Class.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"

#include "gtypes.h"
#include "access.h"
#include "baseClasses.h"
#include "classMethod.h"
#include "constants.h"
#include "exception.h"
#include "itypes.h"
#include "lookup.h"
#include "object.h"
#include "soft.h"
#include "stackTrace.h"
#include "stringSupport.h"
#include "support.h"
#include "reflect.h"

#include "java_io_InputStream.h"
#include "java_io_PrintStream.h"
#include "java_lang_System.h"
#include "java_lang_reflect_Constructor.h"
#include "java_lang_reflect_Method.h"
#include "java_lang_reflect_Field.h"
#include "java_lang_Class.h"

#include "defs.h"

/*
 * Convert string name to class object.
 */
struct Hjava_lang_Class*
java_lang_Class_forName(struct Hjava_lang_String* str,
			jboolean doinit,
			Hjava_lang_ClassLoader* loader)
{
	errorInfo einfo;
	Hjava_lang_Class* clazz;
	Utf8Const *utf8buf;
	const char *buf;
	int jlen;
	jchar *js;

	/*
	 * NB: internally, we store class names as path names (with slashes
	 *     instead of dots.  However, we must also prevent calls to
	 *     "java/lang/Object" or "[[Ljava/lang/Object;" from succeeding.
	 *	Since class names cannot have slashes, we reject all attempts
	 *	to look up names that do.  Awkward.  Inefficient.
	 */
	js = STRING_DATA(str);
	jlen = STRING_SIZE(str);
	while (--jlen > 0) {
		if (*js++ == '/') {
			postExceptionMessage(&einfo,
				JAVA_LANG(ClassNotFoundException),
				"Cannot have slashes - use dots instead.");
			throwError(&einfo);
		}
	}

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
	 * imply that we must avoid asking a class loader to resolve such
	 * array names (those starting with an [), and this is what calling
	 * loadArray does.
	 */

	/* Convert string to utf8, converting '.' to '/' */
	utf8buf = checkPtr(stringJava2Utf8ConstReplace(str, '.', '/'));
	buf = utf8buf->data;

	if (buf[0] == '[') {
		clazz = loadArray(utf8buf, loader, &einfo);
	}
	else {
		clazz = loadClass(utf8buf, loader, &einfo);
	}
	
	/* if an error occurred, throw an exception */
	if (clazz == 0) {
		utf8ConstRelease(utf8buf);
		throwError(&einfo);
	}
	utf8ConstRelease(utf8buf);
	/*
	 * loadClass returns the class in state CSTATE_LINKED.
	 *
	 * Processing to CSTATE_COMPLETE will initialize the class, resolve
	 * its constants and run its static initializers.
	 *
	 * The option to load a class via forName without initializing it
	 * was introduced in 1.2, presumably for the convenience of
	 * programs such as stub compilers.
	 */
	if (doinit && processClass(clazz, CSTATE_COMPLETE, &einfo) == false) {
		throwError(&einfo);
	}
	return (clazz);
}

/*
 * Convert class to string name.
 */
struct Hjava_lang_String*
java_lang_Class_getName(struct Hjava_lang_Class* c)
{
	return(checkPtr(utf8Const2JavaReplace(c->name, '/', '.')));
}

/*
 * Return super class.
 *
 * Note that the specs demands to return null if the class object is an
 * interface or the class object representing java.lang.Object.
 *
 * That is, we're asked to NOT report superinterfaces for interfaces.
 * That would be impossible anyway since the spec says that the super_class
 * attribute in a class file describing an interface must point to
 * java.lang.Object.  An interface is considered to "implement" its
 * superinterface(s).  See also getInterfaceMethods0.
 */
struct Hjava_lang_Class*
java_lang_Class_getSuperclass(struct Hjava_lang_Class* this)
{
	if (!CLASS_IS_INTERFACE(this))
		return (this->superclass);
	else
		return (NULL);
}

HArrayOfObject* /* [Ljava.lang.Class; */
java_lang_Class_getInterfaces(struct Hjava_lang_Class* this)
{
	HArrayOfObject* obj;
	struct Hjava_lang_Class** ifaces;
	int i;
	int nr;

	nr = this->interface_len;

#if defined(JDK_1_1_COMPAT)
	/*
	 * Do not report java.io.Serializable for array classes in JDK 1.1
	 */
	if (CLASS_IS_ARRAY(this)) {
		nr = 0;
	}
#endif

	obj = (HArrayOfObject*)AllocObjectArray(nr, "Ljava/lang/Class;", NULL);
	ifaces = (struct Hjava_lang_Class**)unhand_array(obj)->body;
	for (i = 0; i < nr; i++) {
		ifaces[i] = this->interfaces[i];
	}

	return (obj);
}

/*
 * Return the class loader which loaded me.
 */
struct Hjava_lang_ClassLoader*
java_lang_Class_getClassLoader0(struct Hjava_lang_Class* this)
{
	return (this->loader);
}

/*
 * Is the class an interface?
 */
jboolean
java_lang_Class_isInterface(struct Hjava_lang_Class* this)
{
	return ((this->accflags & ACC_INTERFACE) ? 1 : 0);
}

jboolean
java_lang_Class_isPrimitive(struct Hjava_lang_Class* this)
{
	return (CLASS_IS_PRIMITIVE(this));
}

jboolean
java_lang_Class_isArray(struct Hjava_lang_Class* this)
{
	return (CLASS_IS_ARRAY(this));
}

Hjava_lang_Class*
java_lang_Class_getComponentType(struct Hjava_lang_Class* this)
{
	if (CLASS_IS_ARRAY(this)) {
		return (CLASS_ELEMENT_TYPE(this));
	}
	else {
		return ((Hjava_lang_Class*)0);
	}
}

jboolean
java_lang_Class_isAssignableFrom(struct Hjava_lang_Class* this, struct Hjava_lang_Class* cls)
{
	return (instanceof(this, cls));
}

/*
 * Is object instance of this class?
 */
jboolean
java_lang_Class_isInstance(struct Hjava_lang_Class* this, struct Hjava_lang_Object* obj)
{
	return (soft_instanceof(this, obj));
}

jint
java_lang_Class_getModifiers(struct Hjava_lang_Class* this)
{
#ifndef ACC_SUPER
#define ACC_SUPER ACC_SYNCHRONISED
#endif
	return (this->accflags & (ACC_MASK & ~ACC_SUPER));
}

HArrayOfObject*
java_lang_Class_getSigners0(struct Hjava_lang_Class* this)
{
	return this->signers;
}

void
java_lang_Class_setSigners(struct Hjava_lang_Class* this, HArrayOfObject* sigs)
{
	unhand(this)->signers = sigs;
}

struct Hjava_security_ProtectionDomain*
java_lang_Class_getProtectionDomain (struct Hjava_lang_Class* this)
{
	return unhand(this)->protectionDomain;
}

/*
 * Return true if there is method defined in any subclass of 'cls'
 * that overrides 'meth'.
 *
 * Assumes that base is a subclass of cls
 */
static int
isOverridden(Hjava_lang_Class *base, Hjava_lang_Class *cls, Method *meth)
{
	/* XXX for interfaces for now */
	if (base == 0)
		return (false);

	/* Search superclasses for equivalent method name.
	 * If found extract its index nr.
	 */
	for (; base != cls;  base = base->superclass) {
		int j = CLASS_NMETHODS(base);
		Method* mt = CLASS_METHODS(base);
		for (; --j >= 0;  ++mt) {
			if (utf8ConstEqual (mt->name, meth->name) &&
			    utf8ConstEqual (METHOD_SIG(mt), METHOD_SIG(meth)))
			{
				return (true);
			}
		}
	}
	return (false);
}

/*
 * count the number of methods in a class that are not constructors.
 * If declared is not set, count only public methods.
 */
static int
countMethods(Hjava_lang_Class* base, Hjava_lang_Class* clas, jint declared)
{
	Method* mth = CLASS_METHODS(clas);
	int i;
	int count = 0;

	for (i = CLASS_NMETHODS(clas)-1 ; i >= 0; i--) {
		if (((mth[i].accflags & ACC_PUBLIC) || declared)
		    && !(mth[i].accflags & ACC_CONSTRUCTOR)
		    && !isOverridden(base, clas, mth + i)
		    && !utf8ConstEqual(init_name, mth[i].name)) {
			count++;
		}
	}
	return count;
}

/*
 * create reflect.Method objects for all methods in a class that are
 * not constructors.  If declared is not set, include only public methods.
 */
static void
addMethods(Hjava_lang_Class* base, Hjava_lang_Class* clas, jint declared,
	Hjava_lang_reflect_Method*** ptr)
{
	Method* mth = CLASS_METHODS(clas);
	int i;

	for (i = CLASS_NMETHODS(clas)-1; i >= 0; i--) {
		if (((mth[i].accflags & ACC_PUBLIC) || declared)
		    && !(mth[i].accflags & ACC_CONSTRUCTOR)
		    && !isOverridden(base, clas, mth + i)
		    && !utf8ConstEqual(init_name, mth[i].name)) {
			**ptr = KaffeVM_makeReflectMethod(clas, i);
			(*ptr)++;
		}
	}
}

/*
 * Reflect all methods implemented by an interface or one of its
 * superinterfaces.
 *
 * Note that we do not reach the "superinterface" via the superclass pointer.
 * See the VM Spec, which says:
 *
 *   "The implements clause in a [interface] class declaration lists the
 *    names of interfaces that are direct superinterfaces of the [interface]
 *    class being declared."
 *
 * Hence we must look at the interfaces list for superinterfaces.
 */
static
HArrayOfObject*
getInterfaceMethods0(struct Hjava_lang_Class* this, jint declared)
{
	int count;
	Hjava_lang_reflect_Method** ptr;
	HArrayOfObject* array;
	int i;

	count = 0;
	count += countMethods(NULL, this, declared);
	if (!declared) {
		for (i = 0; i < this->total_interface_len; i++) {
			count += countMethods(NULL, this->interfaces[i], declared);
		}
	}

	array = (HArrayOfObject*)
	    AllocObjectArray(count, "Ljava/lang/reflect/Method;", NULL);
	ptr = (Hjava_lang_reflect_Method**)&unhand_array(array)->body[0];

	addMethods(NULL, this, declared, &ptr);
	if (!declared) {
		for (i = 0; i < this->total_interface_len; i++) {
			addMethods(NULL, this->interfaces[i], declared, &ptr);
		}
	}

	return (array);
}

HArrayOfObject*
java_lang_Class_getMethods0(struct Hjava_lang_Class* this, jboolean declared)
{
	int count;
	Hjava_lang_Class* clas;
	Hjava_lang_reflect_Method** ptr;
	HArrayOfObject* array;

	/*
	 * Note: the spec wants us to include the methods of all superclasses
	 * and all superinterfaces.
	 *
	 * Superinterfaces cannot be reached through the superclass
	 * pointer.  We handle them in a separate function.
	 */
	if (CLASS_IS_INTERFACE(this))
		return (getInterfaceMethods0(this, declared));

	count = 0;
	for (clas = this; clas != NULL; clas = clas->superclass) {

		count += countMethods(this, clas, declared);

		if (declared) {
			break;
		}
	}
	array = (HArrayOfObject*)
	    AllocObjectArray(count, "Ljava/lang/reflect/Method;", NULL);
	ptr = (Hjava_lang_reflect_Method**)&unhand_array(array)->body[0];
	for (clas = this; clas != NULL; clas = clas->superclass) {

		addMethods(this, clas, declared, &ptr);

		if (declared) {
			break;
		}
	}

	return (array);
}

HArrayOfObject*
java_lang_Class_getConstructors0(struct Hjava_lang_Class* this, jboolean declared)
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
	array = (HArrayOfObject*)
	   AllocObjectArray(count, "Ljava/lang/reflect/Constructor;", NULL);
	ptr = (Hjava_lang_reflect_Constructor**)&unhand_array(array)->body[0];
	clas = this;
	mth = CLASS_METHODS(clas);
	for (i = CLASS_NMETHODS(clas)-1; i >= 0;  i--) {
		if (((mth[i].accflags & ACC_PUBLIC) || declared) && (mth[i].accflags & ACC_CONSTRUCTOR)) {
			*ptr = KaffeVM_makeReflectConstructor(clas, i);
			ptr++;
		}
	}
	return (array);
}

/*
 * Count all public fields of a class, including inherited fields and
 * fields from implemented interfaces
 */
static int
countPublicFields(Hjava_lang_Class* clazz)
{
	Hjava_lang_Class* clas;
	int i, count;

	count = 0;
	for (clas = clazz; clas != NULL; clas = clas->superclass) {
		Field *fld = CLASS_FIELDS(clas);
		for (i = CLASS_NFIELDS(clas)-1; i >= 0; i--) {
			if (fld[i].accflags & ACC_PUBLIC) {
				count++;
			}
		}
	}

	for (i = 0; i < clazz->total_interface_len; i++) {
		count += countPublicFields(clazz->interfaces[i]);
	}
	return (count);
}

static void
makePublicFields(Hjava_lang_Class* clazz, jboolean declared, Hjava_lang_reflect_Field*** pptr)
{
	Hjava_lang_Class* clas;
	Hjava_lang_reflect_Field** ptr = *pptr;
	int i;

	for (clas = clazz; clas != NULL; clas = clas->superclass) {
		Field *fld = CLASS_FIELDS(clas);
		for (i = CLASS_NFIELDS(clas)-1; i >= 0;  i--) {
			if ((fld[i].accflags & ACC_PUBLIC) || declared) {
				*ptr = KaffeVM_makeReflectField(clas, i);
				ptr++;
			}
		}
		if (declared) {
			break;
		}
	}
	if (!declared) {
		for (i = 0; i < clazz->total_interface_len; i++) {
			makePublicFields(clazz->interfaces[i], declared, &ptr);
		}
	}
	*pptr = ptr;
}

/*
 * Below, "declared" means to include a field only if it is directly
 * declared by that class (and not inherited from a superclass or defined
 * by an interface the class implements.)  This applies to both private,
 * protected, and public fields.
 *
 * On the other hand, if "declared" is false, we only include public
 * fields.  Weird semantics.
 */
HArrayOfObject*
java_lang_Class_getFields0(struct Hjava_lang_Class* clazz, jboolean declared)
{
	int count;
	Hjava_lang_reflect_Field** ptr;
	HArrayOfObject* array;

	if (declared) {
		count = CLASS_NFIELDS((Hjava_lang_Class*)clazz);
	}
	else {
		count = countPublicFields(clazz);
	}
	array = (HArrayOfObject*)
	    AllocObjectArray(count, "Ljava/lang/reflect/Field;", NULL);
	ptr = (Hjava_lang_reflect_Field**)&unhand_array(array)->body[0];
	makePublicFields(clazz, declared, &ptr);

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
	ssize_t i;
	errorInfo info;

	/* The JDK doc says and experimentation shows that a null second
	 * parameter to all get(Declared){Method|Constructor} functions
	 * is treated like passing an empty array "new Class [] {}"
	 */
	if (argtypes == NULL) {
		if (METHOD_NARGS(mth) == 0) {
			return (1);
		}
		else {
			return (0);
		}
	}

	if (ARRAY_SIZE(argtypes) != METHOD_NARGS(mth)) {
		return (0);
	}

	for (i = 0; i < ARRAY_SIZE(argtypes); i++) {
		Hjava_lang_Class* sigclass;
		sigclass = getClassFromSignaturePart(METHOD_ARG_TYPE(mth, i), mth->class->loader, &info);
		if (sigclass == NULL) {
			discardErrorInfo(&info);
			return 0;
		}
		if (sigclass !=
			(struct Hjava_lang_Class *)OBJARRAY_DATA(argtypes)[i]) {
			return (0);
		}
	}

	return (1);
}

/* Check whether the loading of the class has failed. Throw an NoClassDefFoundError
   in that the case.
*/
static
void
checkIfClassHasFailed(struct Hjava_lang_Class* clas)
{
	if (clas != NULL && clas->state == CSTATE_FAILED) {
		SignalError("java.lang.NoClassDefFoundError", CLASS_CNAME(clas));
	}
}

static
Hjava_lang_reflect_Method*
findMatchingMethod(struct Hjava_lang_Class* clas,
		   struct Hjava_lang_String* name,
		   HArrayOfObject* arr, jboolean declared)
{
	Method* mth = CLASS_METHODS(clas);
	int n = CLASS_NMETHODS(clas);
	int i;
	for (i = 0;  i < n;  ++mth, ++i) {
		if (((mth->accflags & ACC_PUBLIC) || declared)
		    && utf8ConstEqualJavaString(mth->name, name)) {
			if (checkParameters(mth, arr))
				return (KaffeVM_makeReflectMethod(clas, i));
		}
	}
	return (NULL);
}

/** 
 * looks up a declared method.
 * 
 * @param this class where to start looking
 * @param name name of method we're looking for
 * @param arr parameter types of the method
 * @param declared true if the method is supposed to be declared in this class
 * 
 * @return a method, if it can be found. NULL otherwise.
 */
Hjava_lang_reflect_Method*
java_lang_Class_getMethod0(struct Hjava_lang_Class* this, struct Hjava_lang_String* name, HArrayOfObject* arr, jboolean declared)
{
	Hjava_lang_Class* clas;
	Hjava_lang_reflect_Method *rmeth;

	clas = this;
	checkIfClassHasFailed(clas);

	do {
		rmeth = findMatchingMethod(clas, name, arr, declared);
		if (rmeth != 0) {
			return (rmeth);
		}
		clas = clas->superclass;
	} while (!declared && clas != NULL);

	/* If the class is an interface, check implemented interfaces as well.
	 * Those are the interfaces this interface inherited
	 */
	if (CLASS_IS_INTERFACE(this)) {
		int i;
		for (i = 0; i < this->total_interface_len; i++) {
			rmeth = findMatchingMethod(this->interfaces[i],
						   name, arr, declared);
			if (rmeth != 0) {
				return (rmeth);
			}
		}
	}

	return NULL;
}

struct Hjava_lang_reflect_Constructor*
java_lang_Class_getConstructor0(struct Hjava_lang_Class* this, HArrayOfObject* arr, jboolean declared)
{
	Hjava_lang_Class* clas = this;

	Method* mth;
	int n;
	int i;

	checkIfClassHasFailed(clas);

	mth = CLASS_METHODS(clas);
	n = CLASS_NMETHODS(clas);

	for (i = 0;  i < n;  ++mth, ++i) {
		if (((mth->accflags & ACC_PUBLIC) || declared) && (METHOD_IS_CONSTRUCTOR(mth))) {
			if (checkParameters(mth, arr))
				return (KaffeVM_makeReflectConstructor(clas, i));
		}
	}

	SignalError("java.lang.NoSuchMethodException", ""); /* FIXME */
	return NULL;
}

static
Hjava_lang_reflect_Field*
checkForField(struct Hjava_lang_Class* clazz, struct Hjava_lang_String* name, jboolean declared)
{
	int i;
	Hjava_lang_Class* clas;

	/* first try this class's own or inherited fields */
	clas = (Hjava_lang_Class*) clazz;
	do {
		Field* fld = CLASS_FIELDS(clas);
		int n = CLASS_NFIELDS(clas);
		for (i = 0;  i < n;  ++fld, ++i) {
			if (((fld->accflags & ACC_PUBLIC) || declared)
			    && utf8ConstEqualJavaString(fld->name, name)) {
				return KaffeVM_makeReflectField(clas, i);
			}
		}
		clas = clas->superclass;
	} while (!declared && clas != NULL);

	if (!declared) {
		/* then try this class's interfaces fields using the wonders of
		 * recursion.  */
		for (i = 0; i < clazz->total_interface_len; i++) {
			Hjava_lang_reflect_Field *f;
			f = checkForField(clazz->interfaces[i], name, declared);
			if (f != NULL) {
				return (f);
			}
		}
	}
	return (NULL);
}

/* Lookup a field in a class.

   @param clazz class to look in
   @param name a field name
   @param declared true if the field is supposed to be declared in clazz

   @return a pointer to the field, if it can be found. NULL otherwise.
*/
Hjava_lang_reflect_Field*
java_lang_Class_getField0(struct Hjava_lang_Class* clazz, struct Hjava_lang_String* name, jboolean declared)
{
        Hjava_lang_reflect_Field* f = checkForField(clazz, name, declared);
        return (f);
}

HArrayOfObject*
java_lang_Class_getClasses0(struct Hjava_lang_Class* clazz, jboolean inner)
{
	errorInfo einfo;
	int count;
	int i;
	innerClass *ic;
	Hjava_lang_Class* c;
	HArrayOfObject* array;
	Hjava_lang_Class** ptr;

	/* Lookup inner classes or outer class.
	   An inner class have outer == clazz.
	   An outer class have inner == clazz.  */
	count = 0;
	for (i = clazz->nr_inner_classes, ic = clazz->inner_classes; i-- > 0; ic++) {
		if (ic->inner_class == 0 || ic->outer_class == 0) {
			continue;
		}
#if 0
		c = getClass (inner ? ic->outer_class : ic->inner_class, clazz, &einfo);
		if (c == NULL) {
			throwError(&einfo);
		}
		if (c == clazz) {
			count++;
		}
#else
		/* assume one unique constant pool entry per class */
		if (clazz->this_index == (inner ? ic->outer_class : ic->inner_class)) {
			count++;
		}
#endif
	}

	array = (HArrayOfObject*)
		AllocObjectArray(count, "Ljava/lang/Class;", NULL);
	if (count == 0) {
		return array;
	}
	ptr = (Hjava_lang_Class**)&unhand_array(array)->body[0];

	for (i = clazz->nr_inner_classes, ic = clazz->inner_classes; i-- > 0; ic++) {
		if (ic->inner_class == 0 || ic->outer_class == 0) {
			continue;
		}
#if 0
		c = getClass (inner ? ic->outer_class : ic->inner_class, clazz, &einfo);
		if (c == NULL) {
			throwError(&einfo);
		}
		if (c != clazz) {
			continue;
		}
#else
		/* assume one unique constant pool entry per class */
		if (clazz->this_index != (inner ? ic->outer_class : ic->inner_class)) {
			continue;
		}
#endif
		c = getClass (inner ? ic->inner_class : ic->outer_class, clazz, &einfo);
		if (c == NULL) {
			throwError(&einfo);
		}
		*ptr++ = c;
	}
	return array;
}
