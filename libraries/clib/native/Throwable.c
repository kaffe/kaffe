/*
 * java.lang.Throwable.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "lib-license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include <stdio.h>
#include <assert.h>
#include <native.h>
#include "FileDescriptor.h"
#include "FileOutputStream.h"
#include "PrintStream.h"
#include "Throwable.h"
#include "../../../kaffe/kaffevm/support.h"

extern Hjava_lang_Object* buildStackTrace(void*);

/*
 * Fill in stack trace information - don't know what thought.
 */
struct Hjava_lang_Throwable*
java_lang_Throwable_fillInStackTrace(struct Hjava_lang_Throwable* o)
{
	unhand(o)->backtrace = buildStackTrace(0);
	return (o);
}

/*
 * Dump the stack trace to the given stream.
 */
void
java_lang_Throwable_printStackTrace0(struct Hjava_lang_Throwable* o, struct Hjava_lang_Object* p)
{
	int i;
	HArrayOfArray* strarray;

	strarray = (HArrayOfArray*)unhand(o)->backtrace;
	if (strarray != 0) {
		for (i = 0; i < obj_length(strarray); i++) {
			Hjava_lang_Object* line = unhand(strarray)->body[i];
			if (line != 0) {
				do_execute_java_method(p, "println", "([C)V", 0, 0, line);
			}
		}
		do_execute_java_method(p, "flush", "()V", 0, 0);
	}
}
