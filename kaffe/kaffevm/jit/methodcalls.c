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

#include "machine.h"
#include "methodcalls.h"

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
#if defined(COMPARE_AND_EXCHANGE)
	if (COMPARE_AND_EXCHANGE(where, tramp, METHOD_NATIVECODE(meth))) {
		;
	}
#elif defined(ATOMIC_EXCHANGE)
	{
		void *tmp = METHOD_NATIVECODE(meth);

		ATOMIC_EXCHANGE(where, tmp);
	}
#else
#error "You have to define either COMPARE_AND_EXCHANGE or ATOMIC_EXCHANGE"
#endif

#if 0
	if (METHOD_PRE_COMPILED(meth)) {
		nativecode* ncode = METHOD_TRUE_NCODE(meth);
		nativecode* ocode = METHOD_NATIVECODE(meth);
		METHOD_NATIVECODE(meth) = ncode;
		/* Update the dtable entries for all classes if this isn't a
	   	   static method.  */
		if (meth->idx >= 0 && ocode != ncode) {
			meth->class->dtable->method[meth->idx] = ncode;
		}
		SET_METHOD_PRE_COMPILED(meth, 0);
	}
#endif

TDBG(	dprintf("Calling %s:%s%s @ %p\n", meth->class->name->data, meth->name->data, METHOD_SIGD(meth), METHOD_NATIVECODE(meth));	)

DBG(MOREJIT,
	dprintf("soft_fixup_trampoline(): return %p\n",
		METHOD_NATIVECODE(meth));
    );

	return (METHOD_NATIVECODE(meth));
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
	if (CLASS_GCJ((meth)->class) && (meth->accflags & ACC_NATIVE) &&
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
			&& (meth->accflags & ACC_NATIVE)))
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
