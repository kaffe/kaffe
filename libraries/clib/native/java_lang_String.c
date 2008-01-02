/*
 * java.lang.String.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2005, 2006, 2008
 *      Kaffe.org contributors.  See ChangeLog for details. 
 *      All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"

#include "stringSupport.h"
#include "java_lang_String.h"

Hjava_lang_String*
java_lang_VMString_intern(Hjava_lang_String* str)
{
	Hjava_lang_String *ret = stringInternString(str);

	if (!ret) {
		errorInfo info;
		postOutOfMemory(&info);
		throwError(&info);
	}
	return ret;
}
