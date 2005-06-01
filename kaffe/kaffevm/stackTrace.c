/*
 * stackTrace.c
 * Handle stack trace for the interpreter or translator.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 * 
 * Copyright (c) 2003
 *	Mark J. Wielaard <mark@klomp.org>
 *
 * Copyright (c) 2004
 *	Kaffe.org contributors. See ChangeLog for details. All rights reserved.
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
#include "jni_md.h"
#include "gtypes.h"
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
#include "stringSupport.h"

#include "java_lang_StackTraceElement.h"
#include "java_lang_Throwable.h"
#include "java_lang_VMThrowable.h"

static Method*
stacktraceFindMethod (uintp fp, uintp pc);
HArrayOfObject*     getStackTraceElements(struct Hjava_lang_VMThrowable*, struct Hjava_lang_Throwable*);

Hjava_lang_Object*
buildStackTrace(struct _exceptionFrame* base)
{
	int cnt;
	struct _stackTrace trace;
	stackTraceInfo* info;
	struct _exceptionFrame orig;

	(void) orig;			/* avoid compiler warning in intrp */
	DBG(STACKTRACE,
	    dprintf("STACKTRACEINIT(trace, %p, %p, orig);\n", base, base); );
	STACKTRACEINIT(trace, base, base, orig);
	cnt = 0;
	while(STACKTRACEFRAME(trace) && KTHREAD(on_current_stack) ((void *)STACKTRACEFP(trace))) {
		cnt++;
		STACKTRACESTEP(trace);
	}

	/* Build an array of stackTraceInfo */
	info = gc_malloc(sizeof(stackTraceInfo) * (cnt+1), KGC_ALLOC_NOWALK);
	if (!info) {
	    dprintf("buildStackTrace(%p): can't allocate stackTraceInfo\n",
		    base);
	    return NULL;
	}

	cnt = 0;

	DBG(STACKTRACE,
	    dprintf("STACKTRACEINIT(trace, &orig, %p, orig);\n", base); );
	STACKTRACEINIT(trace, &orig, base, orig);

	while (STACKTRACEFRAME(trace) && KTHREAD(on_current_stack) ((void *)STACKTRACEFP(trace))) {
		info[cnt].pc = STACKTRACEPC(trace);
		info[cnt].fp = STACKTRACEFP(trace);
		info[cnt].meth = stacktraceFindMethod (info[cnt].fp, info[cnt].pc);
		cnt++;
		STACKTRACESTEP(trace);
	}
	info[cnt].pc = 0;
	info[cnt].meth = ENDOFSTACK;

	DBG(STACKTRACE,
	    dprintf("ENDOFSTACK\n"); );

	return ((Hjava_lang_Object*)info);
}

#if defined(TRANSLATOR)
#include "machine.h"

static Method*
stacktraceFindMethod(uintp fp UNUSED, uintp _pc)
{
	void *pc_base = KGC_getObjectBase(main_collector, (void *)_pc);

	if (pc_base) {
		jitCodeHeader *jch;

		jch = pc_base;
		return jch->method;
	}
	return NULL;
}

#elif defined(INTERPRETER)

static Method*
stacktraceFindMethod(uintp fp, uintp pc UNUSED)
{
	return (vmExcept_isJNIFrame ((VmExceptHandler *)fp)?0:((VmExceptHandler *)fp)->meth);
}
#endif

static inline int32
getLineNumber(Method* meth, uintp _pc)
{
	size_t i;
	int32 linenr;
	uintp linepc;

	linenr = -1;
	if (meth->lines != 0) {
		linepc = 0;
		for (i = 0; i < meth->lines->length; i++) {
			if (_pc >= meth->lines->entry[i].start_pc
			    && linepc <= meth->lines->entry[i].start_pc) {
				linenr = meth->lines->entry[i].line_nr;
				linepc = meth->lines->entry[i].start_pc;
			}
		}
	}
	return linenr;
}

HArrayOfObject*
getStackTraceElements(struct Hjava_lang_VMThrowable* state,
		      struct Hjava_lang_Throwable* throwable)
{
	int i;
	int frame;
	int first_frame;
	stackTraceInfo* stack;
	HArrayOfObject* result;
	Method* meth;
	Hjava_lang_Class* throwable_class;
	Hjava_lang_StackTraceElement* element;

	if (state == 0) {
		dprintf("VMState for exception is null ... aborting\n");
		KAFFEVM_ABORT();
		KAFFEVM_EXIT(1);
	}

	frame = 0;
	first_frame = 0;
	stack = (stackTraceInfo*)unhand(state)->vmdata;
	throwable_class = ((Hjava_lang_Object*)throwable)->vtable->class;

	for (i = 0; stack[i].meth != ENDOFSTACK; i++) {
		meth = stack[i].meth;
		if (meth != 0 && meth->class != NULL) {
			frame++;
			if (meth->class == throwable_class) {
			  	first_frame = frame;
			}
		}
	}

	result = (HArrayOfObject*)newArray(javaLangStackTraceElement,
					   (jsize)(frame - first_frame));

	frame = 0;
	for(i = 0; stack[i].meth != ENDOFSTACK; i++) {
		meth = stack[i].meth;
		if (meth != 0 && meth->class != NULL) {
			if (frame >= first_frame) {
				element = (Hjava_lang_StackTraceElement*)
					newObject(javaLangStackTraceElement);
				unhand(element)->fileName
					= stringC2Java
					(CLASS_SOURCEFILE(meth->class));
				unhand(element)->lineNumber
					= getLineNumber(meth, stack[i].pc);
				unhand(element)->declaringClass
					= utf8Const2JavaReplace
					(meth->class->name, '/', '.');
				unhand(element)->methodName
					= utf8Const2Java(meth->name);
				unhand(element)->isNative
					= meth->accflags & ACC_NATIVE;
				unhand_array(result)->body[frame - first_frame]
					= (Hjava_lang_Object*)element;
			}
			frame++;
		}
	}

	return result;
}

void
printStackTrace(struct Hjava_lang_Throwable* o,
		struct Hjava_lang_Object* p, int nullOK)
{
	int i;
	Hjava_lang_VMThrowable* vmstate;
	stackTraceInfo* info;
	Method* meth;
	uintp _pc;
	int32 linenr;
	char *buf;
	int len;
	int j;
	Hjava_lang_Object* str;
	jchar* cptr;
	char* class_dot_name;
	errorInfo einfo;

	if (unhand(o)->detailMessage != NULL) {
		char *cstr = checkPtr (stringJava2C(unhand(o)->detailMessage));
		dprintf ("%s\n", cstr);
		KFREE(cstr);
	}

	vmstate = (Hjava_lang_VMThrowable*)unhand(o)->vmState;
	if (vmstate == NULL) {
		return;
	}
	info = (stackTraceInfo*)unhand(vmstate)->vmdata;
	if (info == NULL) {
		return;
	}
	for (i = 0; info[i].meth != ENDOFSTACK; i++) {
		_pc = info[i].pc;
		meth = info[i].meth; 
		if (meth != NULL) {
			linenr = getLineNumber (meth, _pc);
			
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
					sprintf(buf, "   at %s.%s (%s:native)",
						class_dot_name,
						meth->name->data, 
						CLASS_SOURCEFILE(meth->class));
				}
				else {
					sprintf(buf, "   at %s.%s (%s:line unknown, pc %p)",
						class_dot_name,
						meth->name->data, 
						CLASS_SOURCEFILE(meth->class),
						(void*)_pc);
				}
			}
			else {
				sprintf(buf, "   at %s.%s (%s:%d)",
					class_dot_name,
					meth->name->data,
					CLASS_SOURCEFILE(meth->class),
					linenr);
			}
			KFREE(class_dot_name);
			len = strlen(buf);
			str = newArrayChecked(TYPE_CLASS(TYPE_Char), (jsize)len, &einfo);
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
			if (p != NULL || !nullOK) {
				do_execute_java_method(NULL, p, "println",
					"([C)V", NULL, 0, str);
			} else {
				dprintf("%s\n", buf);
			}
			KFREE(buf);
		}
	}
	if (p != NULL || !nullOK) {
		do_execute_java_method(NULL, p, "flush", "()V", NULL, 0);
	}
}
