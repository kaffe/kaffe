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
#include "../../../kaffe/kaffevm/gtypes.h"
#include "../../../kaffe/kaffevm/file.h"
#include "../../../kaffe/kaffevm/readClass.h"
#include "../../../kaffe/kaffevm/constants.h"
#include "../../../kaffe/kaffevm/access.h"
#include "../../../kaffe/kaffevm/classMethod.h"
#include "../../../kaffe/kaffevm/object.h"
#include "../../../kaffe/kaffevm/locks.h"
#include "../../../kaffe/kaffevm/itypes.h"
#include "../../../kaffe/kaffevm/support.h"
#include "../../../kaffe/kaffevm/stringSupport.h"
#include "../../../kaffe/kaffevm/baseClasses.h"
#include "../../../kaffe/kaffevm/exception.h"
#include <native.h>
#include "defs.h"

/*
 * Translate an array of bytes into a class.
 */
struct Hjava_lang_Class*
java_lang_ClassLoader_defineClass0(struct Hjava_lang_ClassLoader* this, struct Hjava_lang_String* name, HArrayOfByte* data, jint offset, jint length)
{
	Hjava_lang_Class* clazz;
	classFile hand;
	classEntry *centry;
	errorInfo info;

	hand.base = &unhand_array(data)->body[offset];
	hand.buf = hand.base;
	hand.size = length;

	clazz = newClass();
	if (clazz == 0) {
		postOutOfMemory(&info);
		throwError(&info);
	}

	/*
	 * Make sure clazz->centry is NULL here, so that nobody will try to
	 * assert that a lock on centry is held during readClass
	 */
	clazz->centry = NULL;
	clazz = readClass(clazz, &hand, this, &info);
	if (clazz == 0) {
		throwError(&info);
	}

	/* 
	 * If a name was given, but the name we found in the class file
	 * was a different one, complain.
	 */
	if (name != NULL) {
		/* The name uses dots, but clazz->name uses slashes */
		Hjava_lang_String *temp =
			checkPtr(utf8Const2JavaReplace(clazz->name, '/', '.')); 

		if (STRING_SIZE(temp) != STRING_SIZE(name) ||
			memcmp(STRING_DATA(temp), STRING_DATA(name), 
			       STRING_SIZE(temp)) != 0) 
		{
			/* 1.2 says: "XXX (wrong name: YYY)" 
			 * where XXX == name and YYY == temp
			 */
			SignalError("java.lang.ClassFormatError", "Wrong name");
		}
	}

	/*
	 * See if an entry for that name and class loader already exists
	 * create one if not.
	 */
	centry = lookupClassEntry(clazz->name, this, &info);
	if (centry == 0) {
		throwError(&info);
	}

	/*
	 * see if somebody loaded that class already
	 */
	lockMutex(centry);
	if (centry->class != NULL) {
		unlockMutex(centry);
		/* 1.2 says: "trying to refine class XXX (bad class loader?)"
		 */
		SignalError("java.lang.ClassFormatError", "Duplicate name");
	}

	/* enter the class we loaded and return */
	centry->class = clazz;
	clazz->centry = centry;
	unlockMutex(centry);

	/*
	 * While it is not necessary that one be able to actually *use*
	 * the returned class object at this point, it is mandatory that
	 * the returned clazz object is a functional Class object.
	 *
	 * The following call will make sure that the returned class object
	 * has its dispatch table set.  The transition PRELOADED->PREPARED 
	 * in processClass sets class->head.dtable.
	 *
	 * Presumably, it shouldn't be necessary here, but is at the
	 * moment - XXX
	 */
	if (processClass(clazz, CSTATE_PREPARED, &info) == false) {
		throwError(&info);
	}
	return (clazz);
}

/*
 * Resolve classes reference by this class.
 */
void
java_lang_ClassLoader_resolveClass0(struct Hjava_lang_ClassLoader* this, struct Hjava_lang_Class* class)
{
	errorInfo info;
	if (processClass(class, CSTATE_LINKED, &info) == false) {
		throwError(&info);
	}
}

/*
 * Load a system class.
 */
struct Hjava_lang_Class*
java_lang_ClassLoader_findSystemClass0(Hjava_lang_ClassLoader* this, Hjava_lang_String* str)
{
	errorInfo info;
	Hjava_lang_Class *clazz = 0;
        int len = STRING_SIZE(str);
        Utf8Const* c;
        char* name;
        char buffer[100];

        if (len <= sizeof(buffer) - 1) {
                name = buffer;
        }
        else {
                name = KMALLOC (len);
        }
	if (name) {
		stringJava2CBuf(str, name, len+1);
		classname2pathname(name, name);

		c = utf8ConstNew(name, len);
		if (c) {
			clazz = loadClass(c, 0, &info);
			utf8ConstRelease(c);
		}
	}

	if (clazz == 0) {
		/* 
		 * upgrade error to an exception if *this* class wasn't found.
		 * See discussion in Class.forName()
		 */
		if (!strcmp(info.classname, "java.lang.NoClassDefFoundError")
		    && !strcmp(info.mess, name))
		{
			errorInfo info_tmp = info;
			postExceptionMessage(&info,
				JAVA_LANG(ClassNotFoundException), 
				info.mess);
			discardErrorInfo(&info_tmp);
		}
		if (name != buffer) {
			KFREE(name);
		}
		throwError(&info);
	}
	if (name != buffer) {
		KFREE(name);
	}

	if (processClass(clazz, CSTATE_COMPLETE, &info) == false) {
		throwError(&info);
	}
	return (clazz);
}

/*
 * Locate the requested resource in the current Jar files and create a
 *  byte array containing it.
 */
HArrayOfByte*
java_lang_ClassLoader_getSystemResourceAsBytes0(struct Hjava_lang_String* str)
{
	char* name;
	char* lname;
	classFile hand;
	HArrayOfByte* data;
	errorInfo einfo;

	lname = name = checkPtr(stringJava2C(str));

	/* skip leading slashes */
	while (*lname && *lname == '/') {
		lname++;
	}
	hand = findInJar(lname, &einfo);
	KFREE(name);
	/* Specs says to return null if the resource wasn't found.
	 * But we return null for any error.
	 */
	if (hand.type == 0) {
		discardErrorInfo(&einfo);
		return (NULL);
	}

	/* Copy data from returned buffer into Java byte array.  Be nice
	 * to avoid this copy but we cannot for the moment.
	 */
	data = (HArrayOfByte*)AllocArray(hand.size, TYPE_Byte);
	memcpy(unhand_array(data)->body, hand.buf, hand.size);
	if (hand.base != NULL) {
		KFREE(hand.base);
	}

	return (data);
}

/*
 * Find a loaded class.
 *
 * Note that this means to find a class for which we were the defining
 * classloader.  We must not report classes for which we were merely the
 * initiating loader.
 */
struct Hjava_lang_Class*
java_lang_ClassLoader_findLoadedClass0(Hjava_lang_ClassLoader* this, Hjava_lang_String* str)
{
        int len = STRING_SIZE(str);
        Utf8Const* c;
        char* name;
        char buffer[100];
        classEntry* entry;
	errorInfo info;

        if (len <= sizeof(buffer) - 1) {
                name = buffer;
        }
        else {
                name = KMALLOC (len);
        }
	if (!name) {
		postOutOfMemory(&info);
		throwError(&info);
	}
        stringJava2CBuf(str, name, len+1);
        classname2pathname(name, name);

        c = utf8ConstNew(name, len);
        if (name != buffer) {
                KFREE(name);
        }
	if (!c) {
		postOutOfMemory(&info);
		throwError(&info);
	}

        entry = lookupClassEntryInternal(c, this);
        utf8ConstRelease(c);

	if (entry != 0) {
		return (entry->class);
	} else {
		return (0);
	}
}

/*
 * Finalize a classloader
 */
void 
java_lang_ClassLoader_finalize0(struct Hjava_lang_ClassLoader*this)
{
	finalizeClassLoader(this);
}
