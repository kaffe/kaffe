/*
 * java.lang.ClassLoader.c
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
#include "file.h"
#include "readClass.h"
#include "constants.h"
#include "access.h"
#include "classMethod.h"
#include "object.h"
#include "locks.h"
#include "itypes.h"
#include "support.h"
#include "stringSupport.h"
#include "baseClasses.h"
#include "exception.h"
#include "java_lang_ClassLoader.h"
#include "java_lang_VMClassLoader.h"
#include "defs.h"

struct Hjava_lang_Class*
java_lang_VMClassLoader_getPrimitiveClass0(jchar typeCode)
{
	switch(typeCode) {
	case 'Z':
		return (booleanClass);
	case 'B':
		return (byteClass);
	case 'C':
		return (charClass);
	case 'D':
		return (doubleClass);
	case 'F':
		return (floatClass);
	case 'I':
		return (intClass);
	case 'J':
		return (longClass);
	case 'S':
		return (shortClass);
	case 'V':
		return (voidClass);
	default:
	  return(NULL);
	}
}

/*
 * Translate an array of bytes into a class.
 */
struct Hjava_lang_Class*
java_lang_VMClassLoader_defineClass(struct Hjava_lang_ClassLoader* this, struct Hjava_lang_String* name,
				    HArrayOfByte* data, jint offset, jint length, struct Hjava_security_ProtectionDomain* protectionDomain)
{
	Hjava_lang_Class *clazz, *dup_clazz;
	classFile hand;
	classEntry *centry;
	errorInfo info;

	/* This is the error sent by JDK 1.4.2 */
	if (length == 0)
		SignalError("java.lang.ClassFormatError", "truncated class");
	if (length < 0)
		SignalError("java.lang.ArrayIndexOutOfBoundsException", "invalid data length"); 
	classFileInit(&hand,
		      NULL,
		      &unhand_array(data)->body[offset],
		      (unsigned) length,
		      CP_BYTEARRAY);

	clazz = newClass();
	if (clazz == 0) {
		postOutOfMemory(&info);
		throwError(&info);
	}

	/*
	 * Convert the class name and set it up, readClass and friends will
	 * check to make sure it agrees with the one in the class file.
	 */
	if (name != NULL) {
		Utf8Const *utf;

		utf = checkPtr(stringJava2Utf8ConstReplace(name, '.', '/'));
		utf8ConstAssign(clazz->name, utf);
	}
	clazz->centry = NULL;
	clazz = readClass(clazz, &hand, this, &info);
	if (clazz == 0) {
		throwError(&info);
	}

	/* set protection domain of new class */
	unhand(clazz)->protectionDomain = protectionDomain;

	/*
	 * See if an entry for that name and class loader already exists
	 * create one if not.
	 */
	centry = lookupClassEntry(clazz->name, this, &info);
	if (centry == 0) {
		throwError(&info);
	}

	if( classMappingLoad(centry, &dup_clazz, &info) )
	{
		if( dup_clazz )
		{
			postExceptionMessage(&info,
					     JAVA_LANG(ClassFormatError),
					     "Duplicate name: %s",
					     centry->name->data);
			throwError(&info);
		}
		/*
		 * While it is not necessary that one be able to actually *use*
		 * the returned class object at this point, it is mandatory
		 * that the returned clazz object is a functional Class object.
		 *
		 * The following call will make sure that the returned class
		 * object has its dispatch table set.  The transition
		 * PRELOADED->PREPARED in processClass sets class->head.dtable.
		 *
		 * Presumably, it shouldn't be necessary here, but is at the
		 * moment - XXX
		 */
		else if( processClass(clazz,
				      CSTATE_PREPARED,
				      &info) == false )
		{
			throwError(&info);
		}
	}
	else
	{
		throwError(&info);
	}
	return (clazz);
}

/*
 * Resolve classes reference by this class.
 */
void
java_lang_VMClassLoader_resolveClass(struct Hjava_lang_Class* class)
{
	errorInfo info;

	if (processClass(class, CSTATE_COMPLETE, &info) == false) {
		throwError(&info);
	}
}


struct Hjava_lang_Class*
java_lang_VMClassLoader_loadClass(Hjava_lang_String* jStr, jboolean resolve)
{
	Hjava_lang_Class *clazz = NULL;
	errorInfo info;
	int error = 0;
        Utf8Const *c;
        char *name;
	int i;
	jboolean foundSlash = false;

	name = checkPtr(stringJava2C(jStr));
	for (i = strlen(name)-1; i >= 0; i--)
		if (name[i] == '/') {
			foundSlash = true;
			break;
		}

	if (!foundSlash)
	  classname2pathname(name, name);

	if (foundSlash ||
	    !strncmp (name, "kaffe/lang/", 11) ||
	    (!strncmp (name, "gnu/classpath/", 14)
             && strncmp (name, 
                         "gnu/classpath/tools/", 
                         strlen("gnu/classpath/tools/")))) {
		struct Hjava_lang_Throwable *throwable;

		throwable = (struct Hjava_lang_Throwable *)execute_java_constructor(JAVA_LANG(ClassNotFoundException),
										    NULL,
										    NULL,
										    "(Ljava/lang/String;)V",
										    jStr);
		throwException (throwable);
	}

	if( (c = utf8ConstNew(name, -1)) )
	{
		clazz = loadClass(c, NULL, &info);
		if( clazz )
		{
			if( processClass(clazz,
					 resolve ? CSTATE_COMPLETE : CSTATE_PREPARED,
					 &info) == false )
			{
				error = 1;
			}
		}
		else
		{
			error = 1;
		}
		utf8ConstRelease(c);
	}
	else
	{
		postOutOfMemory(&info);
		error = 1;
	}

	gc_free(name);
	
	if( error )
	{
		throwError(&info);
	}
	return( clazz );
}

