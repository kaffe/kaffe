/*
 * java.lang.String.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include <native.h>

extern Hjava_lang_String* internJavaString(Hjava_lang_String*);
extern void uninternJavaString(Hjava_lang_String*);

Hjava_lang_String*
java_lang_String_intern0(Hjava_lang_String* str)
{
	return (internJavaString(str));
}

void
java_lang_String_unintern0(Hjava_lang_String* str)
{
	uninternJavaString(str);
}
