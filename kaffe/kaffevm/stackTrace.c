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
#include "debug.h"
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

	(void) orig;			/* avoid compiler warning in intrp */
	DBG(STACKTRACE,
	    dprintf("STACKTRACEINIT(trace, base, base, orig);\n"); );
	STACKTRACEINIT(trace, base, base, orig);
	cnt = 0;
	while(!STACKTRACEEND(trace)) {
		if (!STACKTRACESKIP(trace))
		    cnt++;
		STACKTRACESTEP(trace);
	}

	/* Build an array of stackTraceInfo */
	info = gc_malloc(sizeof(stackTraceInfo) * (cnt+1), GC_ALLOC_NOWALK);
	if (!info) {
	    dprintf("buildStackTrace(%p): can't allocate stackTraceInfo\n",
		    base);
	    return 0;
	}

	cnt = 0;

	DBG(STACKTRACE,
	    dprintf("STACKTRACEINIT(trace, &orig, base, orig);\n"); );
	STACKTRACEINIT(trace, &orig, base, orig);

	for(; !STACKTRACEEND(trace); STACKTRACESTEP(trace)) {
		if (STACKTRACESKIP(trace))
			continue;
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
 * We make these exported functions because we use them in classMethod.c
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
printStackTrace(struct Hjava_lang_Throwable* o,
		struct Hjava_lang_Object* p, int nullOK)
{
	int i;
	stackTraceInfo* info;
	Method* meth;
	uintp pc;
	int32 linenr;
	uintp linepc;
	char *buf;
	int len;
	int j;
	Hjava_lang_Object* str;
	jchar* cptr;
	char* class_dot_name;
	errorInfo einfo;

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
			/* Even if we are reporting an out of memory and
			   checkPtr fails, this is ok.  If we can't allocate
			   a new one, the vm will die in an orderly manner.
			*/
			class_dot_name = checkPtr(
			    KMALLOC(strlen(CLASS_CNAME(meth->class)) + 1));
			pathname2classname(CLASS_CNAME(meth->class), class_dot_name);
			buf = checkPtr(KMALLOC(strlen(class_dot_name)
			    + strlen(meth->name->data)
			    + strlen(CLASS_SOURCEFILE(meth->class))
			    + 64));

			if (linenr == -1) {
				if (meth->accflags & ACC_NATIVE) {
					sprintf(buf, "\tat %s.%s(%s:native)",
						class_dot_name,
						meth->name->data, 
						CLASS_SOURCEFILE(meth->class));
				}
				else {
					sprintf(buf, "\tat %s.%s(%s:line unknown, pc %p)",
						class_dot_name,
						meth->name->data, 
						CLASS_SOURCEFILE(meth->class),
						(void*)pc);
				}
			}
			else {
				sprintf(buf, "\tat %s.%s(%s:%d)",
					class_dot_name,
					meth->name->data,
					CLASS_SOURCEFILE(meth->class),
					linenr);
			}
			KFREE(class_dot_name);
			len = strlen(buf);
			str = newArrayChecked(TYPE_CLASS(TYPE_Char), len, &einfo);
			if (!str) {
				KFREE(buf);
				if (nullOK) {
					return;
				} else {
					throwError(&einfo);
				}
			}
			cptr = (jchar*)OBJARRAY_DATA(str);
			for (j = len;  --j >= 0; ) {
				cptr[j] = (unsigned char)buf[j];
			}
			if (p != 0 || !nullOK) {
				do_execute_java_method(p, "println",
					"([C)V", 0, 0, str);
			} else {
				dprintf("%s\n", buf);
			}
			KFREE(buf);
		}
	}
	if (p != 0 || !nullOK) {
		do_execute_java_method(p, "flush", "()V", 0, 0);
	}
}
