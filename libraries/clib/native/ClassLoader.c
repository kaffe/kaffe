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
#include "java_lang_ClassLoader.h"
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
	int iLockRoot;

	classFileInit(&hand, &unhand_array(data)->body[offset], length, CP_BYTEARRAY);

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

