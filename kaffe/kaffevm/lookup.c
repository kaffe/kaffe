/*
 * lookup.c
 * Various lookup calls for resolving objects, methods, exceptions, etc.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "debug.h"
#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "jtypes.h"
#include "errors.h"
#include "slots.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "classMethod.h"
#include "lookup.h"
#include "exception.h"
#include "thread.h"
#include "baseClasses.h"
#include "stringSupport.h"
#include "machine.h"
#include "locks.h"
#include "soft.h"

static void throwAbstractMethodError(void);

/*
 * Lookup a method reference and get the various interesting bits.
 *
 * Return false if unsuccessful because of a malformed class file
 *	or if the class couldn't be found or processed.
 * Returns true otherwise. 
 *
 * Note that even if it returns true, the method may not be found.
 * call->method is set to NULL in this case.
 */
bool
getMethodSignatureClass(constIndex idx, Hjava_lang_Class* this, bool loadClass, bool isSpecial, callInfo* call, errorInfo *einfo)
{
	constants* pool;
	constIndex ci;
	constIndex ni;
	Hjava_lang_Class* class;
	Utf8Const* name;
	Utf8Const* sig;

	pool = CLASS_CONSTANTS(this);
	if (pool->tags[idx] != CONSTANT_Methodref &&
	    pool->tags[idx] != CONSTANT_InterfaceMethodref) {
DBG(RESERROR,	dprintf("No Methodref found for idx=%d\n", idx);	)
		/* shouldn't that be ClassFormatError or something? */
		SET_LANG_EXCEPTION_MESSAGE(einfo, NoSuchMethodError, 
			"method name unknown")
                return (false);
	}

	ni = METHODREF_NAMEANDTYPE(idx, pool);
	name = WORD2UTF(pool->data[NAMEANDTYPE_NAME(ni, pool)]);
	sig = WORD2UTF(pool->data[NAMEANDTYPE_SIGNATURE(ni, pool)]);

	call->name = name;
	call->signature = sig;

	if (loadClass == false) {
		call->class = 0;
		call->method = 0;
	}
	else {
		ci = METHODREF_CLASS(idx, pool);
		class = getClass(ci, this, einfo);
		if (class == NULL)
			return (false);
		assert(class->state >= CSTATE_LINKED);

                if (isSpecial == true) {
                        if (!utf8ConstEqual(name, constructor_name) && class !=
 this && instanceof(class, this)) {
                                class = this->superclass;
                        }
                }

		call->class = class;
		call->method = 0;
		/* Find method - we don't use findMethod(...) yet since this
		 * will initialise our class (and we don't want to do that).
		 */
		for (; class != 0; class = class->superclass) {
			Method* mptr = findMethodLocal(class, name, sig);
			if (mptr != NULL) {
				call->method = mptr;
				break;
			}
		}
	}

	/* Calculate in's and out's */
	countInsAndOuts(sig->data, &call->in, &call->out, &call->rettype);

DBG(MLOOKUP,	
	if (loadClass) 
		dprintf("getMethodSignatureClass(%s,%s,%s) -> %s\n",
			call->class->name->data, name->data, sig->data, 
			(call->method ? "success" : "failure"));	)
	return (true);
}

/*
 * Get and link the class to which constant pool index idx in class this
 * refers.  The returned class object is at least LINKED.
 */
Hjava_lang_Class*
getClass(constIndex idx, Hjava_lang_Class* this, errorInfo *einfo)
{
	constants* pool;
	Utf8Const *name;
	Hjava_lang_Class* class;
	int tag;
	iLock* lock;

	pool = CLASS_CONSTANTS(this);

	tag = pool->tags[idx];

	switch (tag) {
	case CONSTANT_ResolvedClass:
		return (CLASS_CLASS(idx, pool));

	case CONSTANT_Class:
		/* The class may be resolved by another thread so we better
		 * lock and get the tag & name again just in case.  If we
		 * have been resolved then we just return the answer.
		 */
		lock = lockMutex(this->centry);
		tag = pool->tags[idx];
		name = WORD2UTF(pool->data[idx]);
		unlockKnownMutex(lock);

		if (tag == CONSTANT_ResolvedClass) {
			return (CLASS_CLASS(idx, pool));
		}
		break;

	default:
		SET_LANG_EXCEPTION(einfo, ClassFormatError)
		return NULL;
	}

	/* Find the specified class.
	 * NB: the name is *not* a signature.
	 */
	if (name->data[0] == '[') {
		class = loadArray(name, this->loader, einfo);
	}
	else {
		class = loadClass(name, this->loader, einfo);
	}
	if (class == 0) {	/* propagate failure */
		return NULL;
	}

	/* Lock the class while we update the constant pool.  Someone
	 * may have done this already but we don't care.
	 */
	lock = lockMutex(this->centry);
	pool->data[idx] = (ConstSlot)class;
	pool->tags[idx] = CONSTANT_ResolvedClass;
	unlockKnownMutex(lock);

	return (class);
}

bool
getField(constIndex idx, Hjava_lang_Class* this, bool isStatic, fieldInfo* ret, errorInfo *einfo)
{
	constants* pool;
	constIndex ci;
	constIndex ni;
	Field* field;
	Hjava_lang_Class* class;

	pool = CLASS_CONSTANTS(this);
	if (pool->tags[idx] != CONSTANT_Fieldref) {
DBG(RESERROR,	dprintf("No Fieldref found\n");				)
		SET_LANG_EXCEPTION(einfo, NoSuchFieldError)
		return (false);
	}

	ci = FIELDREF_CLASS(idx, pool);
	class = getClass(ci, this, einfo);
	if (class == NULL)
		return (false);

	ni = FIELDREF_NAMEANDTYPE(idx, pool);

DBG(FLOOKUP,	dprintf("*** getField(%s,%s,%s)\n",
		class->name->data, 
		WORD2UTF(pool->data[NAMEANDTYPE_NAME(ni, pool)])->data, 
		WORD2UTF(pool->data[NAMEANDTYPE_SIGNATURE(ni, pool)])->data);
    )

	field = lookupClassField(class, WORD2UTF(pool->data[NAMEANDTYPE_NAME(ni, pool)]), isStatic, einfo);
	if (field == 0) {
		return (false);
	}

	ret->field = field;
	ret->class = class;
	return (true);
}

/*
 * Lookup a method (and translate) in the specified class.
 */
Method*
findMethodLocal(Hjava_lang_Class* class, Utf8Const* name, Utf8Const* signature)
{
	Method* mptr;
	int n;
	/*
	 * Lookup method - this could be alot more efficient but never mind.
	 * Also there is no attempt to honour PUBLIC, PRIVATE, etc.
	 */
	n = CLASS_NMETHODS(class);
	for (mptr = CLASS_METHODS(class); --n >= 0; ++mptr) {
		if (utf8ConstEqual (name, mptr->name) && utf8ConstEqual (signature, mptr->signature)) {
			if ((mptr->accflags & ACC_ABSTRACT) != 0) {
				SET_METHOD_NATIVECODE(mptr, (void*)throwAbstractMethodError);
				mptr->accflags |= ACC_NATIVE;
			}
			return (mptr);
		}
	}
	return (0);
}

/*
 * Lookup a method (and translate) in the specified class.
 */
Method*
findMethod(Hjava_lang_Class* class, Utf8Const* name, Utf8Const* signature, errorInfo *einfo)
{
	bool success;
	/*
	 * Waz CSTATE_LINKED - Must resolve constants before we do any
	 * translation.  Might not be right though ... XXX
	 */
	if (class->state < CSTATE_USABLE) {
		success = processClass(class, CSTATE_COMPLETE, einfo);
		if (!success)
			return (0);
	}

	/*
	 * Lookup method - this could be alot more efficient but never mind.
	 * Also there is no attempt to honour PUBLIC, PRIVATE, etc.
	 */
	for (; class != 0; class = class->superclass) {
		Method* mptr = findMethodLocal(class, name, signature);
		if (mptr != NULL) {
			return mptr;
		}
	}
	SET_LANG_EXCEPTION_MESSAGE(einfo, NoSuchMethodError, name->data)
	return (0);
}

static
void
throwAbstractMethodError(void)
{
	throwException(AbstractMethodError);
}
