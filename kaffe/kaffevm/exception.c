/*
 * exception.c
 * Handle exceptions for the interpreter or translator.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-signal.h"
#include "config-mem.h"
#include "config-setjmp.h"
#include "config-hacks.h"
#include "jtypes.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "md.h"
#include "classMethod.h"
#include "code.h"
#include "exception.h"
#include "baseClasses.h"
#include "lookup.h"
#include "thread.h"
#include "thread-impl.h"
#include "errors.h"
#include "itypes.h"
#include "external.h"
#include "soft.h"
#include "locks.h"
#include "stackTrace.h"
#include "machine.h"
#include "slots.h"
#include "gcj/gcj.h"

#if defined(INTERPRETER)
#define	FIRSTFRAME(f, e)	/* Does nothing */
#elif defined(TRANSLATOR)
static Method* findExceptionInMethod(uintp, Hjava_lang_Class*, exceptionInfo*);
#endif	/* TRANSLATOR */

static void nullException(struct _exceptionFrame *);
static void floatingException(struct _exceptionFrame *);
static void dispatchException(Hjava_lang_Throwable*, stackTraceInfo*) __NORETURN__;

extern void Kaffe_JNIExceptionHandler(void);

extern void printStackTrace(struct Hjava_lang_Throwable*, struct Hjava_lang_Object*, int);

static bool findExceptionBlockInMethod(uintp, Hjava_lang_Class*, Method*, exceptionInfo*);

/*
 * Create an exception from error information.
 */
Hjava_lang_Throwable*
error2Throwable(errorInfo* einfo)
{
	Hjava_lang_Throwable *err = 0;

	switch (einfo->type & KERR_CODE_MASK) {
	case KERR_EXCEPTION:
		if (einfo->mess == 0 || *einfo->mess == '\0') {
			err = (Hjava_lang_Throwable*)execute_java_constructor(
				    einfo->classname, 0, 0, "()V");
		} else {
			err = (Hjava_lang_Throwable*)execute_java_constructor(
				    einfo->classname,
				    0, 0, "(Ljava/lang/String;)V",
				    checkPtr(stringC2Java(einfo->mess)));
		}
		break;

	case KERR_INITIALIZER_ERROR:
		if (strcmp(CLASS_CNAME(OBJECT_CLASS(&einfo->throwable->base)),
			   "java/lang/ExceptionInInitializerError") != 0) {
			err = (Hjava_lang_Throwable*)execute_java_constructor(
				    JAVA_LANG(ExceptionInInitializerError),
				    0, 0, "(Ljava/lang/Throwable;)V",
				    einfo->throwable);
			break;
		}
		/* FALLTHRU */

	case KERR_RETHROW:
		err = einfo->throwable;
		break;

	case KERR_OUT_OF_MEMORY:
		err = gc_throwOOM();
		break;
	}

	discardErrorInfo(einfo);
	return (err);
}

/*
 * post out-of-memory condition
 */
void
postOutOfMemory(errorInfo *einfo)
{
	memset(einfo, 0, sizeof(*einfo));
    	einfo->type = KERR_OUT_OF_MEMORY;
}

/*
 * post a simple exception using its full name without a message
 */
void
postException(errorInfo *einfo, const char *name)
{
	einfo->type = KERR_EXCEPTION;
	einfo->classname = name;
	einfo->mess = "";
	einfo->throwable = 0;
}

void
vpostExceptionMessage(errorInfo *einfo,
	const char * fullname, const char * fmt, va_list args)
{
        char *msgBuf;
        int msgLen;

	msgBuf = KMALLOC(MAX_ERROR_MESSAGE_SIZE);
	if (msgBuf == 0) {
		einfo->type = KERR_OUT_OF_MEMORY;
		return;
	}

#ifdef HAVE_VSNPRINTF
        msgLen = vsnprintf(msgBuf, MAX_ERROR_MESSAGE_SIZE, fmt, args);
#else
        /* XXX potential buffer overruns problem: */
        msgLen = vsprintf(msgBuf, fmt, args);
#endif
	einfo->type = KERR_EXCEPTION | KERR_FREE_MESSAGE;
	einfo->classname = fullname;
	einfo->mess = msgBuf;
	einfo->throwable = 0;
}

/*
 * post a longer exception with a message using full name
 */
void
postExceptionMessage(errorInfo *einfo,
	const char * fullname, const char * fmt, ...)
{
        va_list args;

        va_start(args, fmt);
	vpostExceptionMessage(einfo, fullname, fmt, args);
        va_end(args);
}

/*
 * post a NoClassDefFoundError - we handle this specially since it might
 * not be a fatal error (depending no where it's generated).
 */
void
postNoClassDefFoundError(errorInfo* einfo, const char* cname)
{
	postExceptionMessage(einfo, JAVA_LANG(NoClassDefFoundError), "%s", cname);
	einfo->type |= KERR_NO_CLASS_FOUND;
}

/*
 * Check whether we threw a NoClassFoundError and if we did clear it.
 * We need this in code-analyse.c to avoid throwing errors when we can't find
 * classes but to terminate when we get real errors.
 */
int
checkNoClassDefFoundError(errorInfo* einfo)
{
	if (einfo->type & KERR_NO_CLASS_FOUND) {
		discardErrorInfo(einfo);
		return (1);
	}
	else {
		return (0);
	}
}

/*
 * dump error info to stderr
 */
void
dumpErrorInfo(errorInfo *einfo)
{
	/* XXX */
}

/*
 * discard the errorinfo, freeing a message if necessary
 */
void
discardErrorInfo(errorInfo *einfo)
{
	if (einfo->type & KERR_FREE_MESSAGE) {
		KFREE(einfo->mess);
		einfo->type &= ~KERR_FREE_MESSAGE;
	}
}

/*
 * Create and throw an exception resulting from an error during VM processing.
 */
void
throwError(errorInfo* einfo)
{
	Hjava_lang_Throwable* eobj = error2Throwable (einfo);
	throwException(eobj);
}

/*
 * Throw an exception with backtrace recomputed.
 *
 * Semantic: take stacktrace right now (overwrite whatever stacktrace
 * is in the exception object) and dispatch.
 */
void
throwException(Hjava_lang_Throwable* eobj)
{
	if (eobj == 0) {
		dprintf("Exception thrown on null object ... aborting\n");
		ABORT();
		EXIT(1);
	}
	unhand(eobj)->backtrace = buildStackTrace(0);
	dispatchException(eobj, (stackTraceInfo*)unhand(eobj)->backtrace);
}

/*
 * Throw an exception without altering backtrace.
 *
 * Semantic: just dispatch from here and leave whatever stacktrace is
 * in the exception object.
 */
void
throwExternalException(Hjava_lang_Throwable* eobj)
{
	if (eobj == 0) {
		dprintf("Exception thrown on null object ... aborting\n");
		ABORT();
		EXIT(1);
	}
	dispatchException(eobj, (stackTraceInfo*)buildStackTrace(0));
}

#if 0
void
throwOutOfMemory(void)
{
	Hjava_lang_Throwable* err;

	err = OutOfMemoryError;
	if (err != NULL) {
		throwException(err);
	}
	dprintf("(Insufficient memory)\n");
	EXIT(-1);
}
#endif

void*
nextFrame(void* fm)
{
#if !defined(SPFRAME)
#define SPFRAME(f)	FPFRAME(f)
#endif

#if defined(TRANSLATOR)
#if defined(STACK_NEXT_FRAME)
	STACK_NEXT_FRAME((exceptionFrame*)fm);
        if (jthread_on_current_stack((void *)SPFRAME((exceptionFrame*)fm))) {
		return (fm);
	}
	else {
		return (0);
	}
#else
        exceptionFrame* nfm;

        nfm = (exceptionFrame*)NEXTFRAME((exceptionFrame*)fm);
        /* Note: this should obsolete the FRAMEOKAY macro */
        if (nfm && jthread_on_current_stack((void *)SPFRAME(nfm))) {
                return (nfm);
        }
        else {
                return (0);
        }
#endif
#else
        vmException* nfm;
        nfm = ((vmException*)fm)->prev;
	return (nfm);
#endif
}

#if defined(TRANSLATOR)

/*
 * Perform all necessary actions to unwind one stack frame
 * and to deliver an exception if a handler exists
 *
 * First determine whether the method is a jitted method or not
 * If not, check if it's handled by JNI.
 * If not, determine whether unlocking must be done and do it.
 * If a handler for this exception exists, dispatch to it
 * Else return the method.
 */
Method*
unwindStackFrame(stackTraceInfo* frame, Hjava_lang_Throwable *eobj)
{
	Method *meth;
	Hjava_lang_Object* obj;
	Hjava_lang_Class* class;
	Hjava_lang_Thread* ct;
	exceptionInfo einfo;

	ct = getCurrentThread();
	class = OBJECT_CLASS(&eobj->base);

	meth = findExceptionInMethod(frame->pc, class, &einfo);

	if (einfo.method == 0 && IS_IN_JNI_RANGE(frame->pc)) {
		Kaffe_JNIExceptionHandler();
	}

	/* Find the sync. object */
	if (einfo.method == 0
		|| (einfo.method->accflags & ACC_SYNCHRONISED) == 0)
	{
		obj = 0;
	}
	else if (einfo.method->accflags & ACC_STATIC) {
		obj = &einfo.class->head;
	}
	else {
		FRAMEOBJECT(obj, frame->fp, einfo);
	}

	/* Handler found - dispatch exception */
	if (einfo.handler != 0) {
		unhand(ct)->exceptObj = 0;
		unhand(ct)->needOnStack = STACK_HIGH;
		CALL_KAFFE_EXCEPTION(frame->fp, einfo.handler, eobj);
	}

	/* If method found and synchronised, unlock the lock */
	if (obj != 0 && (meth->accflags & ACC_SYNCHRONISED) != 0) {
		_slowUnlockMutexIfHeld(&obj->lock, (void*)frame->fp);
	}

	/* If method found and profiler enable, fix self+children time */
#if defined(KAFFE_PROFILER)
	if (profFlag && meth) {
		profiler_click_t end;
		profiler_get_clicks(end);
		meth->totalClicks += end;
	}
#endif
	return (meth);
}
#endif /* defined(TRANSLATOR) */

static
void
dispatchException(Hjava_lang_Throwable* eobj, stackTraceInfo* baseframe)
{
	Hjava_lang_Thread* ct;

#if defined(INTS_DISABLED)
	/*
	 * We should never try to dispatch an exception while interrupts are
	 * disabled.  If the threading system provides a means to do so,
	 * check that we don't attempt to do it anyway.
	 */
	assert(!INTS_DISABLED());
#endif
	ct = getCurrentThread();

	/* Save exception object */
	unhand(ct)->exceptObj = eobj;

#if defined (HAVE_GCJ_SUPPORT)
	/* XXX */
	_Jv_Throw(eobj);
	/* no return */
#endif

	/* Search down exception stack for a match */
DBG(ELOOKUP,
	dprintf ("dispatchException(): %s\n", ((Hjava_lang_Object*)eobj)->dtable->class->name->data);)

#if defined(INTERPRETER)
	{
		Hjava_lang_Object* obj;
		exceptionInfo einfo;
		vmException* frame;
		bool res;

		for (frame = (vmException*)unhand(ct)->exceptPtr; frame != 0; frame = frame->prev) {

			if (frame->meth == (Method*)1) {
				unhand(ct)->exceptPtr = (struct Hkaffe_util_Ptr*)frame;
				Kaffe_JNIExceptionHandler();
			}

			/* Look for handler */
			res = findExceptionBlockInMethod(frame->pc, eobj->base.dtable->class, frame->meth, &einfo);

			/* Find the sync. object */
			if (einfo.method == 0 || (einfo.method->accflags & ACC_SYNCHRONISED) == 0) {
				obj = 0;
			}
			else if (einfo.method->accflags & ACC_STATIC) {
				obj = &einfo.class->head;
			}
			else {
				obj = frame->mobj;
			}

			/* If handler found, call it */
			if (res == true) {
				unhand(ct)->needOnStack = STACK_HIGH;
				frame->pc = einfo.handler;
				JTHREAD_LONGJMP(JTHREAD_ACCESS_JMPBUF(frame, jbuf), 1);
			}

			/* If not here, exit monitor if synchronised. */
			if (obj != 0 && (einfo.method->accflags & ACC_SYNCHRONISED) != 0) {
				_slowUnlockMutexIfHeld(&obj->lock, JTHREAD_ACCESS_JMPBUF(frame, jbuf));
			}
		}
	}
#elif defined(TRANSLATOR)
	{
		stackTraceInfo* frame;

		assert (baseframe != NULL);

		for (frame = baseframe; frame->meth != ENDOFSTACK; frame++) {
			unwindStackFrame(frame, eobj);
		}
	}
#endif
	unhandledException(eobj);
	/* Should not return */
}

void
unhandledException(Hjava_lang_Throwable *eobj)
{
	const char* cname;
	Hjava_lang_Class* class;
	Hjava_lang_Thread* ct;

	ct = getCurrentThread();

	/* Clear held exception object */
	unhand(ct)->exceptObj = 0;

	class = OBJECT_CLASS(&eobj->base);
	cname = CLASS_CNAME(class);

	/* We must catch 'java.lang.ThreadDeath' exceptions now and
	 * kill the thread rather than the machine.
	 */
	if (strcmp(cname, THREADDEATHCLASS) == 0) {
		exitThread();
	}

	/* We don't know what to do here. */
	dprintf("Internal error: caught an unexpected exception.\n"
		"Please check your CLASSPATH and your installation.\n");

	/*
	 * Display the exception and stack trace to help in debugging.
	 */
	{
		Hjava_lang_String *msg;
		msg = unhand((Hjava_lang_Throwable*)eobj)->message;
		if (msg) {
			dprintf("%s: %s\n", cname, stringJava2C(msg));
		} else {
			dprintf("%s\n", cname);
		}
	}
	printStackTrace((Hjava_lang_Throwable*)eobj, 0, 1);
	ABORT();
}

/*
 * Setup the internal exceptions.
 */
void
initExceptions(void)
{
DBG(INIT,
	dprintf("initExceptions()\n");
    )
	/* Catch signals we need to convert to exceptions */
	jthread_initexceptions(nullException, floatingException);
}

/*
 * Null exception - catches bad memory accesses.
 */
static void
nullException(struct _exceptionFrame *frame)
{
	Hjava_lang_Throwable* npe;

	npe = (Hjava_lang_Throwable*)newObject(javaLangNullPointerException);
	unhand(npe)->backtrace = buildStackTrace(frame);
#if defined(HAVE_GCJ_SUPPORT)
	FAKE_THROW_FRAME();
#endif /* defined(HAVE_GCJ_SUPPORT) */
	dispatchException(npe, (stackTraceInfo*)unhand(npe)->backtrace);
}

/*
 * Division by zero.
 */
static void
floatingException(struct _exceptionFrame *frame)
{
	Hjava_lang_Throwable* ae;

	ae = (Hjava_lang_Throwable*)newObject(javaLangArithmeticException);
	unhand(ae)->backtrace = buildStackTrace(frame);
#if defined(HAVE_GCJ_SUPPORT)
	FAKE_THROW_FRAME();
#endif /* defined(HAVE_GCJ_SUPPORT) */
	dispatchException(ae, (stackTraceInfo*)unhand(ae)->backtrace);
}

#if defined(TRANSLATOR)
/*
 * Find exception in method.
 */
static Method *
findExceptionInMethod(uintp pc, Hjava_lang_Class* class, exceptionInfo* info)
{
	Method* ptr;

	info->handler = 0;
	info->class = 0;
	info->method = 0;

	ptr = findMethodFromPC(pc);
	if (ptr != 0) {
		if (findExceptionBlockInMethod(pc, class, ptr, info) == true) {
			return ptr;
		}
	}
	return ptr;
}
#endif

/*
 * Look for exception block in method.
 * Returns true if there is an exception handler, false otherwise.
 */
static bool
findExceptionBlockInMethod(uintp pc, Hjava_lang_Class* class, Method* ptr, exceptionInfo* info)
{
	jexceptionEntry* eptr;
	Hjava_lang_Class* cptr;
	int i;

	assert(class);
	assert(ptr);
	assert(info);

	/* Stash method & class */
	info->method = ptr;
	info->class = ptr->class;

	eptr = &ptr->exception_table->entry[0];

	/* Right method - look for exception */
	if (ptr->exception_table == 0) {
DBG(ELOOKUP,
		dprintf("%s.%s has no handlers.\n", ptr->class->name->data, ptr->name->data); )
		return (false);
	}

DBG(ELOOKUP,
	dprintf("%s.%s has %d handlers (throw was pc=%#x):\n",
		ptr->class->name->data, ptr->name->data,
		ptr->exception_table->length, pc); )

	/*
	 * XXX this list could be sorted so that if pc < start_pc, no
	 * further searching is necessary.
	 */
	for (i = 0; i < ptr->exception_table->length; i++) {
		uintp start_pc = eptr[i].start_pc;
		uintp end_pc = eptr[i].end_pc;
		uintp handler_pc = eptr[i].handler_pc;

DBG(ELOOKUP,	dprintf("  Handler %d covers %#x-%#x\n", i, start_pc, end_pc); )
		if (pc < start_pc || pc > end_pc) {
			continue;
		}

		/* Found exception - is it right type */
		if (eptr[i].catch_idx == 0) {
			info->handler = handler_pc;
DBG(ELOOKUP,		dprintf("  Found handler @ %#x: catches all exceptions.\n", handler_pc); )
			return (true);
		}
		/* Did I try to resolve that catch type before */
		if (eptr[i].catch_type == UNRESOLVABLE_CATCHTYPE) {
DBG(ELOOKUP,		dprintf("  Found handler @ %#x: Unresolvable catch type.\n", handler_pc); )
			return (false);
		}
		/* Resolve catch class if necessary */
		if (eptr[i].catch_type == NULL) {
			/*
			 * XXX Since we pre-load all catch clause exceptions
			 * in code-analyse.c now, this code should never
			 * be called.  Right?
			 */
			errorInfo info;
			eptr[i].catch_type = getClass(eptr[i].catch_idx, ptr->class, &info);
			/*
			 * If we could not resolve the catch class, then we
			 * must a) record that fact to guard against possible
			 * recursive attempts to load it and b) throw the error
			 * resulting from that failure and forget about the
			 * current exception.
			 */
			if (eptr[i].catch_type == NULL) {
DBG(ELOOKUP|DBG_RESERROR,
				dprintf("Couldn't resolve catch class @ cp idx=%d\n",
					eptr[i].catch_idx); )
				eptr[i].catch_type = UNRESOLVABLE_CATCHTYPE;
				throwError(&info);
				return (false);
			}
		}
                for (cptr = class; cptr != 0; cptr = cptr->superclass) {
                        if (cptr == eptr[i].catch_type) {
DBG(ELOOKUP,	dprintf("  Found matching handler at %#x: Handles %s.\n",
			handler_pc, CLASS_CNAME(eptr[i].catch_type)); )
                                info->handler = handler_pc;
                                return (true);
                        }
                }
DBG(ELOOKUP,	dprintf("  Handler at %#x (handles %s), does not match.\n",
			handler_pc, CLASS_CNAME(eptr[i].catch_type)); )
	}
	return (false);
}
