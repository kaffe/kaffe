/*
 * kaffe.management.Classpath.c
 *
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

struct Hjava_lang_String;

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
