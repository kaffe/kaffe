/*
 * exception.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __exception_h
#define __exception_h

#include "config-setjmp.h"

/*
 * Values for Thread.needOnStack.  This variable
 * says how many bytes need to be left on the stack when entering a function
 * call.  When throwing a StackOverflowException, this variable is set to
 * STACK_LOW to have enough space to create the StackOverflowError --- if
 * the error is caught, we set it back to STACK_HIGH.
 */
#define STACK_HIGH      (8*1024)
#define STACK_LOW       256

/* XXX Why is this here?  Its duplicated in a number of places. */
#if defined(__WIN32__)
#define SIG_T   void(*)()
#else
#define SIG_T   void*
#endif

struct _exceptionFrame;
struct Hjava_lang_Class;
struct Hjava_lang_Object;
struct Hjava_lang_Throwable;
struct _methods;
struct _errorInfo;
struct _stackTraceInfo;

#define UNRESOLVABLE_CATCHTYPE	((Hjava_lang_Class*)-1)

typedef struct _jexceptionEntry {
	uintp				start_pc;
	uintp				end_pc;
	uintp				handler_pc;
	constIndex			catch_idx;
	struct Hjava_lang_Class*	catch_type;
} jexceptionEntry;

typedef struct _jexception {
	uint32				length;
	jexceptionEntry			entry[1];
} jexception;

/*
 * A VmExceptHandle is used to handle *any* exception in native code
 * in the core of the VM.  Set up when entering Kaffe_JNI methods, or
 * when callMethodA or callMethodV are invoked.
 *
 * Each thread in the system has a exceptPtr, which points
 * to the most recent VmExceptHandler buffer (the buffers
 * are chained throught the 'prev' field.)
 *
 * In the interpreter *every frame* has a VmExceptHandler
 * associated with it, this is used for catching exceptions at
 * each interpreter stack frame and unlocking synchronized
 * objects if necessary.
 */
typedef struct VmExceptHandler {
	struct VmExceptHandler*		prev;
	struct _methods*		meth;
	union
	{
		/*
		 * Only valid if meth == VMEXCEPTHANDLER_KAFFEJNI_HANDLER 
		 * Only used in JIT:
		 */
		struct
		{
			/* Frame address for JNI entry function. */
			uintp	        		fp;
		} jit;
		/*
		 * The intrp bits are only valid if meth != 0 && meth
		 * != VMEXCEPTHANDLER_KAFFEJNI_HANDLER
		 * Only used in the interpreter:
		 */
		struct
		{
			struct Hjava_lang_Object*	syncobj;
	u4				pc;
		} intrp;
	} frame;
	JTHREAD_DECLARE_JMPBUF		(jbuf);
} VmExceptHandler;

#define VMEXCEPTHANDLER_KAFFEJNI_HANDLER ((struct _methods*)1)

struct _exceptionFrame;

void throwException(struct Hjava_lang_Throwable*) __NORETURN__;
void throwExternalException(struct Hjava_lang_Throwable*) __NORETURN__;
struct Hjava_lang_Throwable* error2Throwable(struct _errorInfo* einfo);
void* nextFrame(void*);

struct Hjava_lang_Object* buildStackTrace(struct _exceptionFrame*);
struct _methods* unwindStackFrame(struct _stackTraceInfo* frame, 
				  struct Hjava_lang_Throwable *eobj);
void unhandledException(struct Hjava_lang_Throwable *eobj) __NORETURN__;

extern void initExceptions(void);

static inline bool vmExcept_isJNIFrame(VmExceptHandler* eh) __UNUSED__;
#if defined(TRANSLATOR)
static inline bool vmExcept_JNIContains(VmExceptHandler* eh, uintp pc) __UNUSED__;
#endif
static inline void vmExcept_setJNIFrame(VmExceptHandler* eh, uintp fp) __UNUSED__;
static inline struct _methods* vmExcept_getMeth(VmExceptHandler* eh) __UNUSED__;
static inline void vmExcept_setMeth(VmExceptHandler* eh, struct _methods* m) __UNUSED__;
static inline void vmExcept_setSyncobj(VmExceptHandler* eh, struct Hjava_lang_Object* syncobj) __UNUSED__;
static inline struct Hjava_lang_Object* vmExcept_getSyncobj(VmExceptHandler* eh) __UNUSED__;
static inline void vmExcept_setPC(volatile VmExceptHandler* eh, u4 pc) __UNUSED__;
static inline u4 vmExcept_getPC(const VmExceptHandler* eh) __UNUSED__;
static inline void vmExcept_jumpToHandler(VmExceptHandler* frame) __UNUSED__ __NORETURN__;

static inline bool
vmExcept_isJNIFrame(VmExceptHandler* eh)
{
	assert(eh);
	return (eh->meth == VMEXCEPTHANDLER_KAFFEJNI_HANDLER);
}

#if defined(TRANSLATOR)
static inline bool
vmExcept_JNIContains(VmExceptHandler* eh, uintp fp)
{
	assert(eh);
	assert(eh->meth == VMEXCEPTHANDLER_KAFFEJNI_HANDLER);
	assert(fp);

	return (eh->frame.jit.fp == fp);
}
#endif

static inline void 
vmExcept_setJNIFrame(VmExceptHandler* eh, uintp fp)
{
	assert(eh);
	assert(fp != 0);

	eh->meth = VMEXCEPTHANDLER_KAFFEJNI_HANDLER;
	eh->frame.jit.fp = fp;
}

static inline void
vmExcept_jumpToHandler(VmExceptHandler* frame)
{
	JTHREAD_LONGJMP(JTHREAD_ACCESS_JMPBUF(frame, jbuf), 1);
}

#if defined(INTERPRETER)
static inline void 
vmExcept_setIntrpFrame(VmExceptHandler* eh, u4 pc, struct _methods* meth, struct Hjava_lang_Object* syncobj)
{
	assert(eh);
	assert(meth);
	
	eh->meth = meth;
	eh->frame.intrp.pc = pc;
	eh->frame.intrp.syncobj = syncobj;
}
#endif

static inline void 
vmExcept_setSyncobj(VmExceptHandler* eh, struct Hjava_lang_Object* syncobj)
{
	assert(eh);
	assert(eh->meth != 0);
	assert(eh->meth != VMEXCEPTHANDLER_KAFFEJNI_HANDLER);
	eh->frame.intrp.syncobj = syncobj;
}

static inline struct Hjava_lang_Object*
vmExcept_getSyncobj(VmExceptHandler* eh)
{
	assert(eh);
	assert(eh->meth != 0);
	assert(eh->meth != VMEXCEPTHANDLER_KAFFEJNI_HANDLER);
	return eh->frame.intrp.syncobj;
}

static inline void 
vmExcept_setMeth(VmExceptHandler* eh, struct _methods* meth)
{
	assert(eh);
	assert(meth);
	eh->meth = meth;
}

static inline struct _methods* 
vmExcept_getMeth(VmExceptHandler* eh)
{
	assert(eh);
	return eh->meth;
}

static inline void 
vmExcept_setPC(volatile VmExceptHandler* eh, u4 pc)
{
	assert(eh);
	assert(eh->meth != 0);
	assert(eh->meth != VMEXCEPTHANDLER_KAFFEJNI_HANDLER);
	eh->frame.intrp.pc = pc;
}

static inline u4 
vmExcept_getPC(const VmExceptHandler* eh)
{
	assert(eh);
	assert(eh->meth != 0);
	assert(eh->meth != VMEXCEPTHANDLER_KAFFEJNI_HANDLER);
	return eh->frame.intrp.pc;
}

#endif



