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
	Hjava_lang_Class *clazz, *dup_clazz;
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
java_lang_ClassLoader_resolveClass0(struct Hjava_lang_ClassLoader* this, struct Hjava_lang_Class* class)
{
	errorInfo info;
	if (processClass(class, CSTATE_COMPLETE, &info) == false) {
		throwError(&info);
	}
}
