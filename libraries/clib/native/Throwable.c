/*
 * java.lang.Throwable.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-io.h"
#include <assert.h>
#include <native.h>
#include "java_io_FileDescriptor.h"
#include "java_io_FileOutputStream.h"
#include "java_io_PrintStream.h"
#include "java_lang_Throwable.h"
#include "../../../kaffe/kaffevm/support.h"

extern Hjava_lang_Object* buildStackTrace(void*);
extern void printStackTrace(struct Hjava_lang_Throwable*, struct Hjava_lang_Object*);

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
	printStackTrace(o, p);
}
