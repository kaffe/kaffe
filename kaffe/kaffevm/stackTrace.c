/*
 * stackTrace.c
 * Handle stack trace for the interpreter or translator.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define	DBG(s)

#include "config.h"
#include "config-std.h"
#include "config-signal.h"
#include "config-mem.h"
#include "config-setjmp.h"
#include "jtypes.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "classMethod.h"
#include "code.h"
#include "exception.h"
#include "baseClasses.h"
#include "lookup.h"
#include "thread.h"
#include "errors.h"
#include "itypes.h"
#include "external.h"
#include "soft.h"
#include "md.h"
#include "stackTrace.h"

/* This file breaks the thread encapsulation model - best fixed by rewriting
 * the thread class.
 */
#include "systems/unix-internal/internal.h"

Hjava_lang_Object*
getClassContext(void* bulk)
{
	int cnt=0;
	int i;
	stackTrace trace;
	Hjava_lang_Object* clazz;
	Method* meth;

	STACKTRACEINIT(trace,NULL,bulk);
	for( ; !STACKTRACEEND(trace) ; STACKTRACESTEP(trace) ) {
		if ( STACKTRACEMETH(trace) != 0 ) {
			cnt++;
		}
	}

	assert( cnt > 0 );

	clazz = newArray(ClassClass, cnt);

	STACKTRACEINIT(trace,NULL,bulk);
	for(i=0;!STACKTRACEEND(trace);STACKTRACESTEP(trace)){
		meth=STACKTRACEMETH(trace);
		if ( meth !=0 ) {
			OBJARRAY_DATA(clazz)[i] =
				(Hjava_lang_Object*)meth->class;
			fprintf(stderr,"class %s, loader %p\n",
				CLASS_CNAME(meth->class),meth->class->loader);
			i++;
		}
	}

	assert( cnt == i );

	return clazz;
}

Hjava_lang_Class*
getClassWithLoader(int* depth)
{
	int cnt;
	stackTrace trace;
	Method* meth;

	cnt = 0;
	STACKTRACEINIT(trace, NULL, depth);
	for(; !STACKTRACEEND(trace); STACKTRACESTEP(trace)) {
		meth = STACKTRACEMETH(trace);
		if (meth != 0) {
			if (meth->class->loader != NULL) {
				*depth = cnt;
				return (meth->class);
			}
			cnt++;
		}
	}
	*depth = -1;
	return (NULL);
}

jint
classDepth(char* name)
{
	int cnt;
	stackTrace trace;
	Method* meth;

	cnt = 0;
	STACKTRACEINIT(trace,NULL,name);
	for( ; !STACKTRACEEND(trace); STACKTRACESTEP(trace) ) {
		meth = STACKTRACEMETH(trace);
		if (meth != 0) {
			if (!strcmp(CLASS_CNAME(meth->class), name)) {
				return (cnt);
			}
			cnt++;
		}
	}
	return(-1);
}

/*
 * Build an array of char[] for the current stack backtrace.
 */
Hjava_lang_Object*
buildStackTrace(struct _exceptionFrame* base)
{
	char buf[200];
	Hjava_lang_Object* str;
	Hjava_lang_Object* strarray;
	int cnt;
	int len;
	int i;
	Method* meth;
	uintp pc;
	int32 linenr;
	uintp linepc;
	struct _stackTrace trace;
	char* esig;
	jchar* cptr;

	STACKTRACEINIT(trace, base, base);
	cnt = 0;
	while(!STACKTRACEEND(trace)) {
		STACKTRACESTEP(trace);
		cnt++;
	}

	/* Build an array of strings */
	strarray = newArray(getClassFromSignature("[C", NULL), cnt);
	assert(strarray != 0);

	cnt = 0;

	STACKTRACEINIT(trace,base,base);

	for(; !STACKTRACEEND(trace); STACKTRACESTEP(trace)) {
		meth = STACKTRACEMETH(trace);
		pc = STACKTRACEPC(trace);

		if (meth != 0) {
			linepc = 0;
			linenr = -1;
			if (meth->lines != 0) {
				for (i = 0; i < meth->lines->length; i++) {
					if (pc >= meth->lines->entry[i].start_pc && linepc < meth->lines->entry[i].start_pc) {
						linenr = meth->lines->entry[i].line_nr;
						linepc = meth->lines->entry[i].start_pc;
					}
				}
			}
			if (linenr == -1) {
				sprintf(buf, "\tat %.80s.%.80s(line unknown, pc %p)",
					CLASS_CNAME(meth->class),
					meth->name->data, (void*)pc);
			}
			else {
				sprintf(buf, "\tat %.80s.%.80s(%d)",
					CLASS_CNAME(meth->class),
					meth->name->data,
					linenr);
			}
			len = strlen(buf);
			str = newArray(TYPE_CLASS(TYPE_Char), len);
			assert(str != 0);
			cptr = (jchar*)OBJARRAY_DATA(str);
			for (i = len;  --i >= 0; ) {
				cptr[i] = (unsigned char) buf[i];
			}
		}
		else {
			str = 0;
		}
		OBJARRAY_DATA(strarray)[cnt] = str;

		cnt++;
	}
	return (strarray);
}
