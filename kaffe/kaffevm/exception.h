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
#include "gtypes.h"

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

struct Hjava_lang_Class;
struct Hjava_lang_Object;
struct Hjava_lang_Throwable;

struct _jmethodID;
struct _errorInfo;

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

#if defined(TRANSLATOR)
typedef uintp                   JNIFrameAddress;
#else
typedef struct VmExceptHandler* JNIFrameAddress;
#endif

/*
 * A VmExceptHandle is used to handle *any* exception in native code
 * in the core of the VM.  Set up when entering Kaffe_JNI methods, or
 * when KaffeVM_callMethodA or KaffeVM_callMethodV are invoked.
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
	struct _jmethodID*		meth;
	union
	{
		/*
		 * Only valid if meth == VMEXCEPTHANDLER_KAFFEJNI_HANDLER 
		 * Used to keep track of jni method invocations.
		 */
		struct
		{
			/* Frame address for JNI entry function. */
			JNIFrameAddress	        		fp;
		} jni;
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
	JTHREAD_JMPBUF		jbuf;
} VmExceptHandler;

#define VMEXCEPTHANDLER_KAFFEJNI_HANDLER ((struct _jmethodID*)1)

void throwException(struct Hjava_lang_Throwable*); 
void throwExternalException(struct Hjava_lang_Throwable*);
struct Hjava_lang_Throwable* error2Throwable(struct _errorInfo* einfo);

void unhandledException(struct Hjava_lang_Throwable *eobj);

extern void initExceptions(void);

bool vmExcept_isJNIFrame(VmExceptHandler* eh);
void vmExcept_setJNIFrame(VmExceptHandler* eh, JNIFrameAddress fp);
void vmExcept_setSyncObj(VmExceptHandler* eh, struct Hjava_lang_Object* syncobj);
void vmExcept_setPC(volatile VmExceptHandler* eh, u4 pc);
u4 vmExcept_getPC(const VmExceptHandler* eh);

#endif



