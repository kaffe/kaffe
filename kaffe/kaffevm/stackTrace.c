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
#include "support.h"


Hjava_lang_Object*
buildStackTrace(struct _exceptionFrame* base)
{
	int cnt;
	struct _stackTrace trace;
	stackTraceInfo* info;
	struct _exceptionFrame orig;

	STACKTRACEINIT(trace, base, base, orig);
	cnt = 0;
	while(!STACKTRACEEND(trace)) {
		STACKTRACESTEP(trace);
		cnt++;
	}

	/* Build an array of stackTraceInfo */
	info = gc_malloc(sizeof(stackTraceInfo) * (cnt+1), GC_ALLOC_NOWALK);
	if (!info) return 0;

	cnt = 0;

	STACKTRACEINIT(trace, &orig, 0, orig);

	for(; !STACKTRACEEND(trace); STACKTRACESTEP(trace)) {
		info[cnt].pc = STACKTRACEPC(trace);
		info[cnt].fp = STACKTRACEFP(trace);
		info[cnt].meth = STACKTRACEMETHCREATE(trace);
		cnt++;
	}
	info[cnt].pc = 0;
	info[cnt].meth = ENDOFSTACK;

	return ((Hjava_lang_Object*)info);
}

/*
 * We make these exported functions because we use them in Class.c
 */
#if defined(TRANSLATOR)
Method*
stacktraceFindMethod(stackTraceInfo *info)
{
	return (findMethodFromPC(info->pc));
}

#elif defined(INTERPRETER)

Method*
stacktraceFindMethod(stackTraceInfo *info)
{
	return (info->meth);
}
#endif

void
printStackTrace(struct Hjava_lang_Throwable* o, struct Hjava_lang_Object* p)
{
	int i;
	stackTraceInfo* info;
	Method* meth;
	uintp pc;
	int32 linenr;
	uintp linepc;
	char buf[256];			/* FIXME: unchecked buffer */
	int len;
	int j;
	Hjava_lang_Object* str;
	jchar* cptr;
	char* class_dot_name;

	info = (stackTraceInfo*)unhand(o)->backtrace;
	if (info == 0) {
		return;
	}
	for (i = 0; info[i].meth != ENDOFSTACK; i++) {
		pc = info[i].pc;
		meth = stacktraceFindMethod(&info[i]);
		if (meth != 0) {
			linepc = 0;
			linenr = -1;
			if (meth->lines != 0) {
				for (j = 0; j < meth->lines->length; j++) {
					if (pc >= meth->lines->entry[j].start_pc && linepc <= meth->lines->entry[j].start_pc) {
						linenr = meth->lines->entry[j].line_nr;
						linepc = meth->lines->entry[j].start_pc;
					}
				}
			}
			class_dot_name = KMALLOC(strlen(CLASS_CNAME(meth->class)) + 1);
			if (!class_dot_name) {
				errorInfo info;
				/* Even if we are reporting an out of
				   memory, this is ok.  If we can't
				   allocate a new one, the vm will
				   die in an orderly manner.
				*/
				postOutOfMemory(&info);
				throwError(&info);
			}
			pathname2classname(CLASS_CNAME(meth->class), class_dot_name);
			if (linenr == -1) {
				if (meth->accflags & ACC_NATIVE) {
					sprintf(buf, "\tat %.80s.%.80s(%s:native)",
						class_dot_name,
						meth->name->data, 
						CLASS_SOURCEFILE(meth->class));
				}
				else {
					sprintf(buf, "\tat %.80s.%.80s(%s:line unknown, pc %p)",
						class_dot_name,
						meth->name->data, 
						CLASS_SOURCEFILE(meth->class),
						(void*)pc);
				}
			}
			else {
				sprintf(buf, "\tat %.80s.%.80s(%s:%d)",
					class_dot_name,
					meth->name->data,
					CLASS_SOURCEFILE(meth->class),
					linenr);
			}
			KFREE(class_dot_name);
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
