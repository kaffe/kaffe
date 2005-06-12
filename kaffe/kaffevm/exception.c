/*
 * exception.c
 * Handle exceptions for the interpreter or translator.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 * 	Mark J. Wielaard <mark@klomp.org>
 *
 * Copyright (c) 2004
 *      Kaffe.org contributors. See ChangeLogs for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#if defined(HAVE_STDARG_H)
#include <stdarg.h>
#endif /* defined(HAVE_STDARG_H) */

#include <stdio.h>

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-signal.h"
#include "config-mem.h"
#include "config-setjmp.h"
#include "config-hacks.h"
#include "jni_md.h"
#include "gtypes.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "md.h"
#include "callKaffeException.h"
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

#include "java_lang_Throwable.h"
#include "java_lang_VMThrowable.h"

#if defined(INTERPRETER)

static struct Hjava_lang_Object*
vmExcept_getSyncObj(VmExceptHandler* eh)
{
	assert(eh != NULL);
	assert(eh->meth != NULL);
	assert(eh->meth != VMEXCEPTHANDLER_KAFFEJNI_HANDLER);
	return eh->frame.intrp.syncobj;
}

#define FRAMEOBJECT(O, F, E)    (O) = vmExcept_getSyncObj((VmExceptHandler*)(F))


#define DISPATCH_EXCEPTION(F, H, E) vmExcept_setPC((VmExceptHandler *)(F), (H));  \
                                    vmExcept_jumpToHandler((VmExceptHandler *)(F)); /* Does not return */
#else

#define DISPATCH_EXCEPTION(F,H,E) thread_data->exceptObj = NULL;\
                                  CALL_KAFFE_EXCEPTION((F),(H),(E));

#endif	/* TRANSLATOR */

static void nullException(struct _exceptionFrame *);
static void floatingException(struct _exceptionFrame *);
static void stackOverflowException(struct _exceptionFrame *);
static void dispatchException(Hjava_lang_Throwable*, stackTraceInfo*);

static bool findExceptionBlockInMethod(uintp, Hjava_lang_Class*, Method*, uintp*);

bool
vmExcept_isJNIFrame(VmExceptHandler* eh)
{
	assert(eh != NULL);
	return (eh->meth == VMEXCEPTHANDLER_KAFFEJNI_HANDLER);
}

static bool
vmExcept_JNIContains(VmExceptHandler* eh, JNIFrameAddress fp)
{
	assert(eh != NULL);
	assert(eh->meth == VMEXCEPTHANDLER_KAFFEJNI_HANDLER);
	assert(fp != (JNIFrameAddress)0);

	return (eh->frame.jni.fp == fp);
}

void 
vmExcept_setJNIFrame(VmExceptHandler* eh, JNIFrameAddress fp)
{
	assert(eh != NULL);
	assert(fp != (JNIFrameAddress)0);

	eh->meth = VMEXCEPTHANDLER_KAFFEJNI_HANDLER;
	eh->frame.jni.fp = fp;
}

static void
vmExcept_jumpToHandler(VmExceptHandler* frame)
{
	JTHREAD_LONGJMP(frame->jbuf, 1);
}

void 
vmExcept_setSyncObj(VmExceptHandler* eh, struct Hjava_lang_Object* syncobj)
{
	assert(eh != NULL);
	assert(eh->meth != NULL);
	assert(eh->meth != VMEXCEPTHANDLER_KAFFEJNI_HANDLER);
	eh->frame.intrp.syncobj = syncobj;
}

void 
vmExcept_setPC(volatile VmExceptHandler* eh, u4 pc)
{
	assert(eh != NULL);
	assert(eh->meth != NULL);
	assert(eh->meth != VMEXCEPTHANDLER_KAFFEJNI_HANDLER);
	eh->frame.intrp.pc = pc;
}

u4 
vmExcept_getPC(const VmExceptHandler* eh)
{
	assert(eh != NULL);
	assert(eh->meth != NULL);
	assert(eh->meth != VMEXCEPTHANDLER_KAFFEJNI_HANDLER);
	return eh->frame.intrp.pc;
}

/*
 * Create an exception from error information.
 */
Hjava_lang_Throwable*
error2Throwable(errorInfo* einfo)
{
	Hjava_lang_Throwable *err = NULL;

	switch (einfo->type & KERR_CODE_MASK) {
	case KERR_EXCEPTION:
		if (einfo->mess == 0 || *einfo->mess == '\0') {
			err = (Hjava_lang_Throwable*)execute_java_constructor(
				    einfo->classname, NULL, NULL, "()V");
		} else {
			err = (Hjava_lang_Throwable*)execute_java_constructor(
				    einfo->classname,
				    NULL, NULL, "(Ljava/lang/String;)V",
				    checkPtr(stringC2Java(einfo->mess)));
		}
		break;

	case KERR_INITIALIZER_ERROR:
		if (strcmp(CLASS_CNAME(OBJECT_CLASS(&einfo->throwable->base)),
			   "java/lang/ExceptionInInitializerError") != 0) {
			err = (Hjava_lang_Throwable*)execute_java_constructor(
				    JAVA_LANG(ExceptionInInitializerError),
				    NULL, NULL, "(Ljava/lang/Throwable;)V",
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
	default:
	        assert(!!!"Unexpected error info mask");
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
	einfo->throwable = NULL;
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
	einfo->throwable = NULL;
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
dumpErrorInfo(errorInfo *einfo UNUSED)
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
		KFREE((void *)einfo->mess);
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
throwException(struct Hjava_lang_Throwable* eobj)
{
	Hjava_lang_VMThrowable* vmstate;
	Hjava_lang_Object* backtrace;

	if (eobj == 0) {
		dprintf("Exception thrown on null object ... aborting\n");
		KAFFEVM_ABORT();
		KAFFEVM_EXIT(1);
	}
	vmstate = unhand(eobj)->vmState;
	if (vmstate == 0) {
		vmstate =
		  (Hjava_lang_VMThrowable*)newObject(javaLangVMThrowable);
		unhand(eobj)->vmState = vmstate;
	}
	backtrace = buildStackTrace(NULL);
	unhand(vmstate)->vmdata = backtrace;
	dispatchException(eobj, (stackTraceInfo*)backtrace);
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
		KAFFEVM_ABORT();
		KAFFEVM_EXIT(1);
	}
	dispatchException(eobj, (stackTraceInfo*)buildStackTrace(NULL));
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
	KAFFEVM_EXIT(-1);
}
#endif

static void
dispatchException(Hjava_lang_Throwable* eobj, stackTraceInfo* baseFrame)
{
	threadData*		thread_data;
	VmExceptHandler*	lastJniFrame;
	stackTraceInfo*		frame;

#if defined(INTS_DISABLED)
	/*
	 * We should never try to dispatch an exception while interrupts are
	 * disabled.  If the threading system provides a means to do so,
	 * check that we don't attempt to do it anyway.
	 */
	assert(!INTS_DISABLED());
#endif
	thread_data = THREAD_DATA(); 

	/* Save exception object */
	thread_data->exceptObj = eobj;

#if defined (HAVE_GCJ_SUPPORT)
	/* XXX */
	_Jv_Throw(eobj); /* no return */
#endif

	/* Search down exception stack for a match */
	DBG(ELOOKUP,
	    dprintf ("dispatchException(): %s\n", ((Hjava_lang_Object*)eobj)->vtable->class->name->data););

	/*
	 * find the last jni frame
	 * (there is _always_ a jni frame somewhere on the stack,
	 *  except during initialiseKaffe() )
	 */
	for (lastJniFrame = thread_data->exceptPtr;
	     lastJniFrame && !vmExcept_isJNIFrame(lastJniFrame);
	     lastJniFrame = lastJniFrame->prev);

	DBG(ELOOKUP,
		dprintf ("dispatchException(): lastJniFrame is %p, fp %p\n", lastJniFrame, (lastJniFrame?lastJniFrame->frame.jni.fp:0)); );

	/*
	 * now walk up the stack 
	 */
	for (frame = baseFrame; frame->meth != ENDOFSTACK; frame++) {
		bool 			foundHandler;
		uintp 			handler = 0;
		Hjava_lang_Object*	obj;

		/*
		 * if we reach the last jni frame, we're done
		 */
		if (lastJniFrame && vmExcept_JNIContains(lastJniFrame, frame->fp)) {
			thread_data->exceptPtr = lastJniFrame;
			vmExcept_jumpToHandler(lastJniFrame); /* doesn't return */
		}

		/*
		 * if we could not determine the java method of this stack frame,
		 * simply ignore that frame
		 */
		if (frame->meth == 0) {
			continue;
		}

		/*
		 * check whether that method contains a suitable handler
		 */
		foundHandler = findExceptionBlockInMethod(frame->pc,
							  eobj->base.vtable->class,
							  frame->meth,
							  &handler);

		/* Find the sync. object */
		if ((frame->meth->accflags & ACC_SYNCHRONISED)==0) {
			obj = NULL;
		} else if (frame->meth->accflags & ACC_STATIC) {
			obj = &frame->meth->class->head;
		} else {
			FRAMEOBJECT(obj, frame->fp, frame->meth);
		}

		/* If handler found, call it */
		if (foundHandler) {
			thread_data->needOnStack = STACK_HIGH;
			DISPATCH_EXCEPTION(frame->fp, handler, eobj); /* doesn't return */
		}

#if defined(ENABLE_JVMPI)
		soft_exit_method(frame->meth);
#endif

		/* If not here, exit monitor if synchronised. */
		if (frame->meth->accflags & ACC_SYNCHRONISED) {
			locks_internal_slowUnlockMutexIfHeld(&obj->lock, NULL);
		}	    

		/* If method found and profiler enable, fix self+children time */
#if defined(TRANSLATOR) && defined(KAFFE_PROFILER)
		if (profFlag) {
			profiler_click_t end;
			profiler_get_clicks(end);
			frame->meth->totalClicks += end;
		}
#endif    
	}

	/*
	 * we did not find a handler...
	 */
	unhandledException (eobj);
}

void
unhandledException(Hjava_lang_Throwable *eobj)
{
	const char* cname;
	Hjava_lang_Class* class;

	/* Clear held exception object */
	THREAD_DATA()->exceptObj = NULL;

	class = OBJECT_CLASS(&eobj->base);
	cname = CLASS_CNAME(class);

	/* We don't know what to do here. */
	dprintf("Internal error: caught an unexpected exception.\n"
		"Please check your CLASSPATH and your installation.\n");

	/*
	 * Display the exception and stack trace to help in debugging.
	 */
	{
		Hjava_lang_String *msg;
		msg = unhand((Hjava_lang_Throwable*)eobj)->detailMessage;
		if (msg) {
			dprintf("%s: %s\n", cname, stringJava2C(msg));
		} else {
			dprintf("%s\n", cname);
			if (strcmp(cname, "java/lang/StackOverflowError") == 0)
			  dprintf("This error may occur because the stack size is not sufficient. \n"
				  "Try to increase the stack size using 'ulimit -s' or with the '-ss'\n"
				  "option on kaffe.\n");
		}
	}
	printStackTrace((Hjava_lang_Throwable*)eobj, NULL, 1);
	KAFFEVM_ABORT();
}

/*
 * Setup the internal exceptions.
 */
void
initExceptions(void)
{
DBG(INIT,
	dprintf("initExceptions()\n");
    );
	/* Catch signals we need to convert to exceptions */
	KTHREAD(initexceptions)(nullException, floatingException, stackOverflowException);
}

/*
 * Stack Overflow exception - catches stack overflows.
 */
static void
stackOverflowException(struct _exceptionFrame *frame)
{
  	Hjava_lang_Throwable* soe;
	Hjava_lang_VMThrowable* vmstate;
	Hjava_lang_Object* backtrace;

	soe = (Hjava_lang_Throwable*)newObject(javaLangStackOverflowError);
	vmstate = (Hjava_lang_VMThrowable*)newObject(javaLangVMThrowable);
	backtrace = buildStackTrace(frame);
	unhand(vmstate)->vmdata = backtrace;
	unhand(soe)->vmState = vmstate;
#if defined(HAVE_GCJ_SUPPORT)
	FAKE_THROW_FRAME();
#endif /* defined(HAVE_GCJ_SUPPORT) */
	dispatchException(soe, (stackTraceInfo*)backtrace);
}

/*
 * Null exception - catches bad memory accesses.
 */
static void
nullException(struct _exceptionFrame *frame)
{
	Hjava_lang_Throwable* npe;
	Hjava_lang_VMThrowable* vmstate;
	Hjava_lang_Object* backtrace;

	npe = (Hjava_lang_Throwable*)newObject(javaLangNullPointerException);
	vmstate = (Hjava_lang_VMThrowable*)newObject(javaLangVMThrowable);
	backtrace = buildStackTrace(frame);
	unhand(vmstate)->vmdata = backtrace;
	unhand(npe)->vmState = vmstate;
	unhand(npe)->cause = npe;
#if defined(HAVE_GCJ_SUPPORT)
	FAKE_THROW_FRAME();
#endif /* defined(HAVE_GCJ_SUPPORT) */
	dispatchException(npe, (stackTraceInfo*)backtrace);
}

/*
 * Division by zero.
 */
static void
floatingException(struct _exceptionFrame *frame)
{
	Hjava_lang_Throwable* ae;
	Hjava_lang_VMThrowable* vmstate;
	Hjava_lang_Object* backtrace;

	ae = (Hjava_lang_Throwable*)newObject(javaLangArithmeticException);
	vmstate = (Hjava_lang_VMThrowable*)newObject(javaLangVMThrowable);
	backtrace = buildStackTrace(frame);
	unhand(vmstate)->vmdata = backtrace;
	unhand(ae)->vmState = vmstate;
#if defined(HAVE_GCJ_SUPPORT)
	FAKE_THROW_FRAME();
#endif /* defined(HAVE_GCJ_SUPPORT) */
	dispatchException(ae, (stackTraceInfo*)backtrace);
}

/*
 * Look for exception block in method.
 * Returns true if there is an exception handler, false otherwise.
 *
 * Passed 'pc' is the program counter where the exception entered
 * the current frame (the 'throw' or from a nested method call).
 */
static bool
findExceptionBlockInMethod(uintp _pc, Hjava_lang_Class* class, Method* ptr, uintp* handler)
{
	jexceptionEntry* eptr;
	Hjava_lang_Class* cptr;
	unsigned int i;

	assert(handler);

	/* Right method - look for exception */
	if (ptr->exception_table == 0) {
DBG(ELOOKUP,
		dprintf("%s.%s has no handlers.\n", ptr->class->name->data, ptr->name->data); );
		return (false);
	}

	eptr = &ptr->exception_table->entry[0];

DBG(ELOOKUP,
	dprintf("%s.%s has %d handlers (throw was pc=%#lx):\n",
		ptr->class->name->data, ptr->name->data,
		ptr->exception_table->length, (long) _pc); );

	for (i = 0; i < ptr->exception_table->length; i++) {
		uintp start_pc = eptr[i].start_pc;
		uintp end_pc = eptr[i].end_pc;
		uintp handler_pc = eptr[i].handler_pc;

DBG(ELOOKUP,	dprintf("  Handler %d covers %#lx-%#lx catches %s\n", i, 
			(long) start_pc, (long) end_pc, eptr[i].catch_type==NULL?"all":CLASS_CNAME(eptr[i].catch_type)););
		if (_pc < start_pc || _pc >= end_pc) {
			continue;
		}

		/* Found exception - is it right type */
		if (eptr[i].catch_idx == 0) {
			*handler = handler_pc;
DBG(ELOOKUP,		dprintf("  Found handler @ %#lx: catches all exceptions.\n", 
				(long) handler_pc); );
			return (true);
		}
		/* Did I try to resolve that catch type before */
		if (eptr[i].catch_type == UNRESOLVABLE_CATCHTYPE) {
DBG(ELOOKUP,		dprintf("  Found handler @ %#lx: Unresolvable catch type.\n", 
				(long) handler_pc); );
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
					eptr[i].catch_idx); );
				eptr[i].catch_type = UNRESOLVABLE_CATCHTYPE;
				throwError(&info);
				return (false);
			}
		}
                for (cptr = class; cptr != 0; cptr = cptr->superclass) {
                        if (cptr == eptr[i].catch_type) {
DBG(ELOOKUP,	dprintf("  Found matching handler at %#lx: Handles %s.\n",
			(long) handler_pc, CLASS_CNAME(eptr[i].catch_type)); );
                                *handler = handler_pc;
                                return (true);
                        }
                }
DBG(ELOOKUP,	dprintf("  Handler at %#lx (handles %s), does not match.\n",
			(long) handler_pc, CLASS_CNAME(eptr[i].catch_type)); );
	}
	return (false);
}
