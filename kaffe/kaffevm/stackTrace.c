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

#define	ENDOFSTACK		((Method*)-1)
typedef struct _stackTraceInfo {
	uintp	pc;
	Method*	meth;
} stackTraceInfo;

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
 * Build an array of stackTraceInfo[] for the current stack backtrace.
 */
Hjava_lang_Object*
buildStackTrace(struct _exceptionFrame* base)
{
	int cnt;
	struct _stackTrace trace;
	stackTraceInfo* info;

	STACKTRACEINIT(trace, base, base);
	cnt = 0;
	while(!STACKTRACEEND(trace)) {
		STACKTRACESTEP(trace);
		cnt++;
	}

	/* Build an array of stackTraceInfo */
	info = gc_malloc(sizeof(stackTraceInfo) * (cnt+1), GC_ALLOC_NOWALK);

	cnt = 0;

	STACKTRACEINIT(trace,base,base);

	for(; !STACKTRACEEND(trace); STACKTRACESTEP(trace)) {
		info[cnt].pc = STACKTRACEPC(trace);
#if defined(INTERPRETER)
		info[cnt].meth = STACKTRACEMETH(trace);
#endif
#if defined(TRANSLATOR)
		info[cnt].meth = 0;	/* We do this lazily */
#endif
		cnt++;
	}
	info[cnt].pc = 0;
	info[cnt].meth = ENDOFSTACK;

	return ((Hjava_lang_Object*)info);
}

void
printStackTrace(struct Hjava_lang_Throwable* o, struct Hjava_lang_Object* p)
{
	int i;
	stackTraceInfo* info;
	Method* meth;
	uintp pc;
	int32 linenr;
	uintp linepc;
	char buf[200];
	int len;
	int j;
	Hjava_lang_Object* str;
	jchar* cptr;

	info = (stackTraceInfo*)unhand(o)->backtrace;
	if (info == 0) {
		return;
	}
	for (i = 0; info[i].meth != ENDOFSTACK; i++) {
		pc = info[i].pc;
#if defined(INTERPRETER)
		meth = info[i].meth; 
#endif
#if defined(TRANSLATOR)
		meth = findMethodFromPC(pc);
#endif
		if (meth != 0) {
			linepc = 0;
			linenr = -1;
			if (meth->lines != 0) {
				for (j = 0; j < meth->lines->length; j++) {
					if (pc >= meth->lines->entry[j].start_pc && linepc < meth->lines->entry[j].start_pc) {
						linenr = meth->lines->entry[j].line_nr;
						linepc = meth->lines->entry[j].start_pc;
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
			cptr = (jchar*)OBJARRAY_DATA(str);
			for (j = len;  --j >= 0; ) {
				cptr[j] = (unsigned char)buf[j];
			}
			do_execute_java_method(p,"println","([C)V",0,0,str);
		}
	}
	do_execute_java_method(p, "flush", "()V", 0, 0);
}
