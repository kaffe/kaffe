/*
 * methodcalls.c
 * Implementation of method calls
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *      The kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */
#include "config.h"
#if defined(HAVE_LIBFFI)
#include "sysdepCallMethod-ffi.h"
#else
#define NEED_sysdepCallMethod 1
#endif

#include "locks.h"
#include "machine.h"
#include "methodcalls.h"
#include "thread.h"
#include "slots.h"
#include "soft.h"
#include "external.h"
#include "jni_i.h"

void *
engine_buildTrampoline (Method *meth, void **where, errorInfo *einfo UNUSED)
{
	*where = meth;
	return meth;
}

static void
startJNIcall(void)
{
	threadData 	*thread_data = THREAD_DATA();
	jnirefs* table;

	table = gc_malloc
	  (sizeof(jnirefs) + sizeof(jref)*DEFAULT_JNIREFS_NUMBER,
	   KGC_ALLOC_STATIC_THREADDATA);

	table->prev = thread_data->jnireferences;
	thread_data->jnireferences = table;
	table->frameSize = DEFAULT_JNIREFS_NUMBER;
	table->localFrames = 1;

	/* No pending exception when we enter JNI routine */
	thread_data->exceptObj = NULL;
}

static void
finishJNIcall(void)
{
	jref eobj;
	threadData	*thread_data = THREAD_DATA();
	jnirefs* table;
	int localFrames;

	table = thread_data->jnireferences;
	localFrames = table->localFrames;
	for (localFrames = table->localFrames; localFrames >= 1; localFrames--)
	  {
	    thread_data->jnireferences = table->prev;
	    gc_free(table);
	    table = thread_data->jnireferences;
	  }

	/* If we have a pending exception, throw it */
	eobj = thread_data->exceptObj;
	if (eobj != 0) {
		thread_data->exceptObj = NULL;
		throwExternalException(eobj);
	}
}

void
engine_callMethod (callMethodInfo *call)
{
	Method *meth = (Method *)call->function;

	if ((meth->accflags & ACC_NATIVE) == 0) {
		virtualMachine(meth, (slots*)(call->args+2), (slots*)call->ret, THREAD_DATA()); 
	}
	else {
		Hjava_lang_Object* syncobj = 0;
		VmExceptHandler mjbuf;
		threadData* thread_data = THREAD_DATA(); 
		struct Hjava_lang_Throwable *save_except = NULL;
		errorInfo einfo;

		if (!METHOD_TRANSLATED(meth)) {
			nativecode *func = native(meth, &einfo);
			if (func == NULL) {
				throwError(&einfo);
			}
			setMethodCodeStart(meth, func);
			meth->accflags |= ACC_TRANSLATED;
		}

		call->function = getMethodCodeStart(meth);

		if (meth->accflags & ACC_JNI)
		{
			if (meth->accflags & ACC_STATIC)
			{
				call->callsize[1] = PTR_TYPE_SIZE / SIZEOF_INT;
				call->argsize += call->callsize[1];
				call->calltype[1] = 'L';
				call->args[1].l = meth->class;
			}
			else
			{
				call->nrargs -= 1;
				call->args += 1;
				call->callsize += 1;
				call->calltype += 1;
			}

			/* Insert the JNI environment */
			call->callsize[0] = PTR_TYPE_SIZE / SIZEOF_INT;
			call->calltype[0] = 'L';
			call->args[0].l = THREAD_JNIENV(); 
			call->argsize += call->callsize[0]; 
		}
		else
		{
			call->nrargs -= 2;
			call->args += 2;
			call->callsize += 2;
			call->calltype += 2;
		}

		if (meth->accflags & ACC_SYNCHRONISED) {
			if (meth->accflags & ACC_STATIC) {
				syncobj = &meth->class->head;
			}
			else if (meth->accflags & ACC_JNI) {
				syncobj = (Hjava_lang_Object*)call->args[1].l;
			}
			else {
				syncobj = (Hjava_lang_Object*)call->args[0].l;
			}

			lockObject(syncobj);
		}

		/* Put 0 in the biggest field. This is needed for some architecture which stores
		 * the bytes out of order. */
		if (call->ret != NULL)
		  call->ret->j = 0;

		setupExceptionHandling(&mjbuf, meth, syncobj, thread_data);

		/* This exception has yet been handled by the VM creator.
		 * We are putting it in stand by until it is cleared. For
		 * that JNI call we're cleaning up the pointer and we will
		 * put it again to the value afterward.
		 */
		if ((meth->accflags & ACC_JNI) != 0) {
			if (thread_data->exceptObj != NULL)
				save_except = thread_data->exceptObj;
			else
				save_except = NULL;
			startJNIcall();
		}

		/* Make the call - system dependent */
		sysdepCallMethod(call);

		if (syncobj != 0) {
			unlockObject(syncobj);
		}

		/* If we have a pending exception and this is JNI, throw it */
		if ((meth->accflags & ACC_JNI) != 0) {
		        if (call->rettype == 'L')
			        call->ret->l = unveil(call->ret->l);
		        finishJNIcall();
			thread_data->exceptObj = save_except;
		}

		cleanupExceptionHandling(&mjbuf, thread_data);
	}

}

