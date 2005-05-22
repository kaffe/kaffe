/*
 * java.lang.Throwable.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 * Copyright (c) 2003
 *	Mark J. Wielaard <mark@klomp.org>
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-io.h"
#include <assert.h>
#include <native.h>
#include "java_lang_StackTraceElement.h"
#include "java_lang_Throwable.h"
#include "java_lang_VMThrowable.h"

extern Hjava_lang_Object* buildStackTrace(void*);
extern HArrayOfObject* getStackTraceElements(struct Hjava_lang_VMThrowable*,
					     struct Hjava_lang_Throwable*);

/*
 * Fill in stack trace information - don't know what thought.
 */
void
java_lang_VMThrowable_fillInStackTrace(struct Hjava_lang_VMThrowable* o)
{
	unhand(o)->vmdata = buildStackTrace(NULL);
	assert(unhand(o)->vmdata != NULL);
}

/*
 * Returns a array of StackTraceElements for the given VMThrowable state.
 */
HArrayOfObject*
java_lang_VMThrowable_getStackTrace(struct Hjava_lang_VMThrowable* state, struct Hjava_lang_Throwable* throwable)
{
	return getStackTraceElements(state, throwable);
}
