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
#include <glib.h>

#if defined(HAVE_LIBFFI)
#include "sysdepCallMethod-ffi.h"
#else
#define NEED_sysdepCallMethod 1
#endif

#include "machine.h"
#include "methodcalls.h"
#include "thread.h"
#include "callKaffeException.h"

#define TDBG(s)

/*
 * Trampolines come in here - do the translation and replace the trampoline.
 */
nativecode*
soft_fixup_trampoline(FIXUP_TRAMPOLINE_DECL)
{
	Method* meth;
	void **where;
	void *tramp;
	void *nativeCode;
	errorInfo info;

	/* FIXUP_TRAMPOLINE_INIT sets tramp and where */
	FIXUP_TRAMPOLINE_INIT;
	tramp = *where;

DBG(MOREJIT,
	dprintf("soft_fixup_trampoline(): meth %p, where %p, native code %p\n",
		meth, where, PMETHOD_NATIVECODE(meth));
    );

	/* If this class needs initializing, do it now.  */
	if (meth->class->state != CSTATE_COMPLETE &&
		processClass(meth->class, CSTATE_COMPLETE, &info) == false) {
		throwError(&info);
	}

	/* Generate code on demand.  */
	if (!METHOD_TRANSLATED(meth)) {
		if (!translate(meth, &info)) {
			throwError(&info);
		}
	}

	/*
	 * Update the origin of the trampoline and free it if necessary. 
	 * Another thread might have jumped through the same trampoline
	 * while we were translating the method, so we have to make this
	 * atomic.
	 */
	nativeCode = METHOD_NATIVECODE(meth);
	g_atomic_pointer_compare_and_exchange(where, tramp, nativeCode);

TDBG(	dprintf("Calling %s:%s%s @ %p\n", meth->class->name->data, meth->name->data, METHOD_SIGD(meth), nativeCode);	)

DBG(MOREJIT,
	dprintf("soft_fixup_trampoline(): return %p\n", nativeCode);
    );

	return nativeCode;
}

/*
 * When do we need a trampoline?
 *
 * NB: this method is invoked for *all* methods a class defines or
 * inherits.
 */
static bool
methodNeedsTrampoline(Method *meth)
{
 	/* A gcj class's native virtual methods always need a trampoline
	 * since the gcj trampoline doesn't work for them.  By using a
	 * trampoline, we can fix the vtable the first time it is invoked.
	 *
	 * NB: If we'll ever support CNI, revisit this.
	 */
	if (CLASS_GCJ((meth)->class) && methodIsNative(meth) &&
		meth->idx != -1)
		return (true);

	/* If the method hasn't been translated, we need a trampoline
	 * NB: we assume the TRANSLATED flag for abstract methods produced
	 * by gcj is cleared.
	 */
	if (!METHOD_TRANSLATED(meth))
		return (true);

	/* We also need one if it's a static method and the class
	 * hasn't been initialized, because such method invocation
	 * would constitute a first active use, requiring the initializer
	 * to be run.
	 */
	if ((meth->accflags & ACC_STATIC)
		&& meth->class->state < CSTATE_DOING_INIT)
	{
		/* Exception: gcj's classes don't need trampolines for two
		 * reasons:
 		 *   a) call _Jv_InitClass before invoking any static method.
		 *   b) they're not compiled as indirect invocations anyway
		 */
		if (!CLASS_GCJ(meth->class)) {
			return (true);
		}
	}
	return (false);
}

void *
engine_buildTrampoline (Method *meth, void **where, errorInfo *einfo)
{
	void *ret;
	methodTrampoline *tramp;

	if (methodNeedsTrampoline(meth)) {
		/* XXX don't forget to pick those up at class gc time */
		tramp = (methodTrampoline*)gc_malloc(sizeof(methodTrampoline), KGC_ALLOC_TRAMPOLINE);
		if (tramp == NULL) {
			postOutOfMemory(einfo);
			return (NULL);
		}
		FILL_IN_TRAMPOLINE(tramp, meth, where);

		/* a disadvantage of building trampolines individually---as
		 * opposed to allocating them in a contiguous region---is that
		 * we have flush the dcache individually for each trampoline
		 */
		FLUSH_DCACHE(tramp, tramp+1);

		/* for native gcj methods, we do override their
		 * anchors so we can patch them up before they're invoked.
		 */
		if (!(CLASS_GCJ((meth)->class)
			&& methodIsNative(meth)))
		{
			assert(*where == NULL ||
				!!!"Cannot override trampoline anchor");
		}
		ret = tramp;
		//		gc_add_ref(tramp);
	} else {
		if (CLASS_GCJ((meth)->class)) {
			_SET_METHOD_NATIVECODE(meth, meth->ncode);
		}
		assert(METHOD_NATIVECODE(meth) != NULL);
		ret = METHOD_NATIVECODE(meth);
	}
	*where = ret;
	return ret;
}

void
engine_callMethod (callMethodInfo *call)
{
	/* Clear the long value to clear the all return value. */
	if (call->ret != NULL)
	  call->ret->j = 0;
	sysdepCallMethod (call);
}

void
engine_dispatchException (uintp framePointer, uintp handler, 
			  struct Hjava_lang_Throwable *throwable)
{
  /* I do not like the following line. I think that the interpreter
   * can be adapted so that we do not need to include that line in the JIT.
   */
  THREAD_DATA()->exceptObj = NULL;
  CALL_KAFFE_EXCEPTION(framePointer, handler, throwable);
}
