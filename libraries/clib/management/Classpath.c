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
#include "../../../kaffe/kaffevm/gtypes.h"
#include "../../../kaffe/kaffevm/support.h"
#include "../../../kaffe/kaffevm/stringSupport.h"
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
	KFREE(s);
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
	KFREE(s);
}

