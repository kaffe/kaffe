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
#include "config-std.h"
#include "../../../kaffe/kaffevm/stringSupport.h"
#include <native.h>

Hjava_lang_String*
java_lang_String_intern0(Hjava_lang_String* str)
{
	return(stringInternString(str));
}
