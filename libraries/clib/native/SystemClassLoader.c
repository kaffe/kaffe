/*
 * kaffe.lang.SystemClassLoader.c
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
#include "kaffe_lang_SystemClassLoader.h"
#include <native.h>
#include "defs.h"

/*
 * Load a system class.
 */
struct Hjava_lang_Class*
kaffe_lang_SystemClassLoader_findClass0(Hkaffe_lang_SystemClassLoader* this, Hjava_lang_String* str)
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

	/* do not process to COMPLETE, it will run static initializers
	 * prematurely!
	 */
	if (processClass(clazz, CSTATE_LINKED, &info) == false) {
		throwError(&info);
	}
	return (clazz);
}

