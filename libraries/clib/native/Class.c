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
#include "access.h"

#include "java_lang_reflect_Constructor.h"
#include "java_lang_reflect_Method.h"
#include "java_lang_reflect_Field.h"
#include "java_lang_VMClass.h"

#include "defs.h"

/*
 * Convert string name to class object.
 */
struct Hjava_lang_Class*
java_lang_VMClass_forName(struct Hjava_lang_String* str, jboolean initialize, struct Hjava_lang_ClassLoader* loader)
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
	if (initialize && processClass(clazz, CSTATE_COMPLETE, &einfo) == false) {
		throwError(&einfo);
	}
	return (clazz);
}

struct Hjava_lang_Class*
java_lang_VMClass_loadArrayClass(struct Hjava_lang_String* str, struct Hjava_lang_ClassLoader* loader)
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

	/* Convert string to utf8, converting '.' to '/' */
	utf8buf = checkPtr(stringJava2Utf8ConstReplace(str, '.', '/'));
	buf = utf8buf->data;
	clazz = loadArray(utf8buf, loader, &einfo);

	/* if an error occurred, throw an exception */
	if (clazz == 0) {
		utf8ConstRelease(utf8buf);
		throwError(&einfo);
	}
	utf8ConstRelease(utf8buf);

	return clazz;
}

void
java_lang_VMClass_initialize(struct Hjava_lang_Class* c)
{
	errorInfo einfo;

	if (processClass(c, CSTATE_COMPLETE, &einfo) == false) {
		throwError(&einfo);
	}
}

/*
 * Convert class to string name.
 */
struct Hjava_lang_String*
java_lang_VMClass_getName(struct Hjava_lang_Class* c)
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
java_lang_VMClass_getSuperclass(struct Hjava_lang_Class* this)
{
	if (!CLASS_IS_INTERFACE(this))
		return (this->superclass);
	else
		return (NULL);
}

HArrayOfObject* /* [Ljava.lang.Class; */
java_lang_VMClass_getInterfaces(struct Hjava_lang_Class* this)
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
java_lang_VMClass_getClassLoader(struct Hjava_lang_Class* this)
{
	return (this->loader);
}

/*
 * Is the class an interface?
 */
jboolean
java_lang_VMClass_isInterface(struct Hjava_lang_Class* this)
{
	return ((this->accflags & ACC_INTERFACE) ? 1 : 0);
}

jboolean
java_lang_VMClass_isPrimitive(struct Hjava_lang_Class* this)
{
	return (CLASS_IS_PRIMITIVE(this));
}

jboolean
java_lang_VMClass_isArray(struct Hjava_lang_Class* this)
{
	return (CLASS_IS_ARRAY(this));
}

Hjava_lang_Class*
java_lang_VMClass_getComponentType(struct Hjava_lang_Class* this)
{
	if (CLASS_IS_ARRAY(this)) {
		return (Kaffe_get_array_element_type(this));
	}
	else {
		return ((Hjava_lang_Class*)NULL);
	}
}

jboolean
java_lang_VMClass_isAssignableFrom(struct Hjava_lang_Class* this, struct Hjava_lang_Class* cls)
{
	if (cls == NULL)
		return false;

	return (instanceof(this, cls));
}

/*
 * Is object instance of this class?
 */
jboolean
java_lang_VMClass_isInstance(struct Hjava_lang_Class* this, struct Hjava_lang_Object* obj)
{
	return (soft_instanceof(this, obj));
}

jint
java_lang_VMClass_getModifiers(struct Hjava_lang_Class* this,
			       jboolean ignoreInnerClassAttribute)
{
#ifndef ACC_SUPER
#define ACC_SUPER ACC_SYNCHRONISED
#endif
	accessFlags accflags = this->accflags;
	
	if (this->this_inner_index >= 0 && !ignoreInnerClassAttribute)
	{
		assert(this->inner_classes != NULL);
		accflags = this->inner_classes[this->this_inner_index].inner_class_accflags;
	}
	return accflags & (ACC_MASK & ~ACC_SUPER);
}

HArrayOfObject*
java_lang_VMClass_getDeclaredMethods(struct Hjava_lang_Class* clazz, jboolean publicOnly)
{
	int count;
	Hjava_lang_reflect_Method** ptr;
	HArrayOfObject* array;
	int i;
	Method *mth = Kaffe_get_class_methods(clazz);

	count = 0;
	for (i = CLASS_NMETHODS(clazz)-1; i >= 0; i--) {
		if (publicOnly && ((mth[i].accflags&ACC_PUBLIC)==0))
			continue;
		
		if ((mth[i].kFlags & KFLAG_CONSTRUCTOR)!=0)
			continue;

		if (utf8ConstEqual(init_name, mth[i].name))
			continue;

		count++;
	}

	array = (HArrayOfObject*)
	    AllocObjectArray(count, "Ljava/lang/reflect/Method;", NULL);
	ptr = (Hjava_lang_reflect_Method**)&unhand_array(array)->body[0];

	for (i = CLASS_NMETHODS(clazz)-1; i >= 0; i--) {
		if (publicOnly && ((mth[i].accflags&ACC_PUBLIC)==0))
			continue;

		if ((mth[i].kFlags & KFLAG_CONSTRUCTOR)!=0)
			continue;

		if (utf8ConstEqual(init_name, mth[i].name))
			continue;

		*ptr = KaffeVM_makeReflectMethod(clazz, i);
		ptr++;
	}

	return (array);
}

HArrayOfObject*
java_lang_VMClass_getDeclaredConstructors(struct Hjava_lang_Class* clas, jboolean publicOnly)
{
	int count;
	Hjava_lang_reflect_Constructor** ptr;
	HArrayOfObject* array;
	int i;
	Method* mth = Kaffe_get_class_methods(clas);

	count = 0;
	for (i = CLASS_NMETHODS(clas)-1; i >= 0;  i--) {
		if ((mth[i].kFlags & KFLAG_CONSTRUCTOR) == 0)
			continue;

		if (publicOnly && ((mth[i].accflags&ACC_PUBLIC)==0))
			continue;
		
		count++;
	}
	array = (HArrayOfObject*)
	   AllocObjectArray(count, "Ljava/lang/reflect/Constructor;", NULL);
	ptr = (Hjava_lang_reflect_Constructor**)&unhand_array(array)->body[0];

	for (i = CLASS_NMETHODS(clas)-1; i >= 0;  i--) {
		if ((mth[i].kFlags & KFLAG_CONSTRUCTOR) == 0)
			continue;

		if (publicOnly && ((mth[i].accflags&ACC_PUBLIC)==0))
			continue;

		*ptr = KaffeVM_makeReflectConstructor(clas, i);
		ptr++;
	}
	return (array);
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
java_lang_VMClass_getDeclaredFields(struct Hjava_lang_Class* clazz, jboolean publicOnly)
{
	int count;
	Hjava_lang_reflect_Field** ptr;
	HArrayOfObject* array;
	int i;
	Field *fld = CLASS_FIELDS(clazz);

	if (publicOnly) {
		count = 0;
		for (i = CLASS_NFIELDS(clazz)-1; i >= 0; i--) {
			if (publicOnly && ((fld[i].accflags&ACC_PUBLIC)==0))
				continue;
			
			count++;
		}
	} else {
		count = CLASS_NFIELDS(clazz);
	}

	array = (HArrayOfObject*)
	    AllocObjectArray(count, "Ljava/lang/reflect/Field;", NULL);
	ptr = (Hjava_lang_reflect_Field**)&unhand_array(array)->body[0];

	for (i = CLASS_NFIELDS(clazz)-1; i >= 0;  i--) {
		if (publicOnly && ((fld[i].accflags & ACC_PUBLIC)==0))
			continue;

		
		*ptr = KaffeVM_makeReflectField(clazz, i);
		ptr++;
	}

	return (array);
}

Hjava_lang_Class*
java_lang_VMClass_getDeclaringClass(struct Hjava_lang_Class* this)
{
	errorInfo		einfo;
	Hjava_lang_Class 	*ret = NULL;

	if (unhand(this)->this_inner_index >= 0) {
		innerClass	*ic = unhand(this)->inner_classes;
		int oc = ic[unhand(this)->this_inner_index].outer_class;

		if (oc == 0)
		  return NULL;

		ret = getClass (oc,
				this,
				&einfo);

		if (ret == NULL)
			throwError(&einfo);
	}
	return ret;
}


HArrayOfObject*
java_lang_VMClass_getDeclaredClasses(struct Hjava_lang_Class* clazz, jboolean publicOnly)
{
	errorInfo einfo;
	int count;
	int i;
	innerClass *ic;
	Hjava_lang_Class* c;
	HArrayOfObject* array;
	Hjava_lang_Class** ptr;

	/* if we don't have inner classes, bail out */
	if (clazz->nr_inner_classes == 0)
		return (HArrayOfObject*)AllocObjectArray(0, "Ljava/lang/Class;", NULL);

	count = 0;
	for (i = clazz->nr_inner_classes, ic = clazz->inner_classes; i-- > 0; ic++) {
		/* assume one unique constant pool entry per class */
		if (ic->outer_class != clazz->this_index)
			continue;
		if (publicOnly && ((ic->inner_class_accflags&ACC_PUBLIC)==0))
			continue;

		count++;
	}

	array = (HArrayOfObject*)
		AllocObjectArray(count, "Ljava/lang/Class;", NULL);

	ptr = (Hjava_lang_Class**)&unhand_array(array)->body[0];

	for (i = clazz->nr_inner_classes, ic = clazz->inner_classes; i-- > 0; ic++) {
		/* assume one unique constant pool entry per class */
		if (ic->outer_class != clazz->this_index)
			continue;
		if (publicOnly && ((ic->inner_class_accflags&ACC_PUBLIC)==0))
			continue;

		c = getClass (ic->inner_class, clazz, &einfo);
		if (c == NULL) {
			throwError(&einfo);
		}
		*ptr++ = c;
	}
	return array;
}


void
java_lang_VMClass_throwException (struct Hjava_lang_Throwable *throwable)
{
	throwExternalException(throwable);
}

struct Hjava_lang_Class*
java_lang_VMClass_getEnclosingClass(struct Hjava_lang_Class* klass)
{
  errorInfo einfo;
  struct Hjava_lang_Class *enclosing;

  if (klass->enclosingClassIndex == 0)
    return NULL;

  enclosing = getClass(klass->enclosingClassIndex, klass, &einfo);
  if (enclosing == NULL)
    discardErrorInfo(&einfo);

  return enclosing;
}

struct Hjava_lang_reflect_Constructor*
java_lang_VMClass_getEnclosingConstructor(struct Hjava_lang_Class* klass)
{
  errorInfo einfo;
  constants *pool;
  constIndex emi = klass->enclosingMethodIndex;
  constIndex eci = klass->enclosingClassIndex;
  constIndex ni;
  Utf8Const *name, *sig;
  Hjava_lang_Class *enclosingClass;
  Method *method;
  int methodSlot;

  if (emi == 0)
    return NULL;

  pool = CLASS_CONSTANTS (klass);

  enclosingClass = getClass(eci, klass, &einfo);
  if (enclosingClass == NULL)
    {
      discardErrorInfo(&einfo);
      return NULL;
    }
  
  switch (pool->tags[emi])
    {
    case CONSTANT_Methodref:
      ni = METHODREF_NAMEANDTYPE(emi, pool);
      name = WORD2UTF(pool->data[NAMEANDTYPE_NAME(ni, pool)]);
      sig = WORD2UTF(pool->data[NAMEANDTYPE_SIGNATURE(ni, pool)]);
      method = findMethodLocal(enclosingClass, name, sig);
      if (method != NULL)
	{
	  lockClass (klass);
	  if (pool->tags[emi] != CONSTANT_ResolvedMethod)
	    {
	      pool->tags[emi] = CONSTANT_ResolvedMethod;
	      pool->data[emi] = (ConstSlot)method;
	    }
	  else
	    method = (Method*)pool->data[emi];
	  unlockClass (klass);
	}
      break;
    case CONSTANT_ResolvedMethod:
      method = (Method *)pool->data[emi];
      break;
    default:
      return NULL;
    }

  if (method == NULL || !METHOD_IS_CONSTRUCTOR (method))
    return NULL;

  methodSlot = method - Kaffe_get_class_methods(klass);
  assert(methodSlot < CLASS_NMETHODS(klass));
  
  return KaffeVM_makeReflectConstructor(klass, methodSlot);
}

struct Hjava_lang_reflect_Method*
java_lang_VMClass_getEnclosingMethod(struct Hjava_lang_Class* klass)
{
  errorInfo einfo;
  constants *pool;
  constIndex emi = klass->enclosingMethodIndex;
  constIndex eci = klass->enclosingClassIndex;
  constIndex ni;
  Utf8Const *name, *sig;
  Hjava_lang_Class *enclosingClass;
  Method *method;
  int methodSlot;

  if (emi == 0)
    return NULL;

  pool = CLASS_CONSTANTS (klass);

  enclosingClass = getClass(eci, klass, &einfo);
  if (enclosingClass == NULL)
    {
      discardErrorInfo(&einfo);
      return NULL;
    }
  
  switch (pool->tags[emi])
    {
    case CONSTANT_Methodref:
      ni = METHODREF_NAMEANDTYPE(emi, pool);
      name = WORD2UTF(pool->data[NAMEANDTYPE_NAME(ni, pool)]);
      sig = WORD2UTF(pool->data[NAMEANDTYPE_SIGNATURE(ni, pool)]);
      method = findMethodLocal(enclosingClass, name, sig);
      if (method != NULL)
	{
	  lockClass (klass);
	  if (pool->tags[emi] != CONSTANT_ResolvedMethod)
	    {
	      pool->tags[emi] = CONSTANT_ResolvedMethod;
	      pool->data[emi] = (ConstSlot)method;
	    }
	  else
	    method = (Method*)pool->data[emi];
	  unlockClass (klass);
	}
      break;
    case CONSTANT_ResolvedMethod:
      method = (Method *)pool->data[emi];
      break;
    default:
      return NULL;
    }

  if (method == NULL || METHOD_IS_CONSTRUCTOR (method))
    return NULL;

  methodSlot = method - Kaffe_get_class_methods(klass);
  assert(methodSlot < CLASS_NMETHODS(klass));
  
  return KaffeVM_makeReflectMethod(klass, methodSlot);
}

struct Hjava_lang_String*
java_lang_VMClass_getClassSignature(struct Hjava_lang_Class* klass)
{
  if (klass->extSignature == NULL)
    return NULL;

  return utf8Const2Java(klass->extSignature);
}

jboolean
java_lang_VMClass_isAnonymousClass(struct Hjava_lang_Class* klass)
{
  /* We do not follow the specification exactly here. However we may want to be
   * compatible. Testcase ?
   */
  return ((klass->kFlags & KFLAG_ANONYMOUS) != 0);
}


jboolean
java_lang_VMClass_isLocalClass(struct Hjava_lang_Class* klass)
{
  return (klass->enclosingClassIndex != 0);
}

jboolean
java_lang_VMClass_isMemberClass(struct Hjava_lang_Class* klass)
{
  return (klass->this_inner_index >= 0);
}

