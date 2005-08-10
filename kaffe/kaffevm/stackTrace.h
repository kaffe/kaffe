/*
 * stackTrace.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __stacktrace_h
#define __stacktrace_h

#include "stackTrace-impl.h"

struct _jmethodID;

/*
 * A backtrace is modelled as an array of stackTraceInfo structs,
 * with index 0 being the first function on the stack. It is
 * terminated by an entry, whose meth field has the special value
 * ENDOFSTACK.
 */
typedef struct _stackTraceInfo {
	uintp   pc;
	uintp	fp;
	struct _jmethodID* meth;
} stackTraceInfo;

#define ENDOFSTACK	((struct _jmethodID*)-1)

Hjava_lang_Object*	buildStackTrace(struct _exceptionFrame*);
void			printStackTrace(struct Hjava_lang_Throwable*, struct Hjava_lang_Object*, int);

#endif
