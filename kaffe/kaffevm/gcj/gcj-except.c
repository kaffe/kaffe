/*
 * gcj-except.c
 * ...
 *
 * Copyright (c) 1996, 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "config-std.h"
#include "config-hacks.h"
#include "config-setjmp.h"
#include "gtypes.h"
#include "classMethod.h"
#include "stackTrace.h"
#include "thread.h"
#include "gcj-except.h"
#include "stringSupport.h"
#include "lookup.h"
#include "md.h"
/*
#include "gcj.h"
*/

#if defined(HAVE_GCJ_SUPPORT)

struct _exceptionFrame;

/* From libgcc2.c */
extern void (*__terminate_func)(void);
extern void __throw(void);

/*
 * See if the exception info matches the given class.
 */
static
int
gcjMatcher(gcjException* einfo, void* type, void* table)
{
	Hjava_lang_Class* cptr;
	Utf8Const* tname;

	if (type == 0) {
		return (1);
	}

	tname = (Utf8Const*)((uintp)type-1);

	for (cptr = einfo->class; cptr != 0; cptr = cptr->superclass) {
		if (utf8ConstEqual(tname, cptr->name)) {
			return (1);
		}
	}
	return (0);
}

static jmp_buf env;	/* Not yet thread safe */

static
void
gcjCatcher(void)
{
	longjmp(env, 1);
}

/*
 * Dispatch a GCJ exception.
 */
void
gcjDispatchException(stackTraceInfo* frame, exceptionInfo* einfo, Hjava_lang_Throwable* eobj)
{
	void** q;
	gcjException* p;
	void (*oldfunc)(void);

	p = jmalloc(sizeof(gcjException));

	p->class = OBJECT_CLASS(&eobj->base);
	p->eh_info.match_function = gcjMatcher;
	p->eh_info.language = GCJ_LANGUAGECODE;
	p->eh_info.version = GCJ_VERSIONCODE;
	
	q = __get_eh_info();
	*q = p;
	oldfunc = __terminate_func;
	__terminate_func = gcjCatcher;
	if (setjmp(env) == 0) {
		/* CALL_EXCEPTION_HANDLER(method, (uintp)__throw, frame); */
		__throw();
	}
	/* Failed */
	__terminate_func = oldfunc;
}

/*
 * Return the exception object.
 */
void*
_Jv_exception_info(void)
{
	Hjava_lang_Thread* ct;
	void* obj;

        ct = getCurrentThread();
	obj = unhand(ct)->exceptObj;
	unhand(ct)->exceptObj = 0;
	
	return (obj);
}

#endif
