/*
 * kaffe.management.Classpath.c
 *
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include <native.h>
#include "gtypes.h"
#include "support.h"
#include "stringSupport.h"
#include "kaffe_management_Classpath.h"

/*
 * Add a JAR/ZIP to the classpath.
 */
void
kaffe_management_Classpath_add0(struct Hjava_lang_String* str)
{
	char *s;

	s = checkPtr(stringJava2C(str));
	addClasspath(s);
	gc_free(s);
}

/*
 * Prepend a JAR/ZIP to the classpath.
 */
void
kaffe_management_Classpath_prepend0(struct Hjava_lang_String* str)
{
	char *s;

	s = checkPtr(stringJava2C(str));
	prependClasspath(s);
	gc_free(s);
}

