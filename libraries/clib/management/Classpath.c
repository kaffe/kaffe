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

/*
 * Add a JAR/ZIP to the classpath.
 */
void
kaffe_management_Classpath_add(struct Hjava_lang_String* str)
{
	char buf[1024];

	javaString2CString(str, buf, sizeof(buf));

	addClasspath(buf);
}
