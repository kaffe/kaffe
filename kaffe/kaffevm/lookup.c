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
#include "machine.h"
#include "locks.h"
#include "soft.h"

static void throwAbstractMethodError(void);
static void throwNoSuchMethodError(void);

/*
 * Lookup a method reference and get the various interesting bits.
 */
void
getMethodSignatureClass(constIndex idx, Hjava_lang_Class* this, bool loadClass, bool isSpecial, callInfo* call)
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
DBG(MLOOKUP,	dprintf("No Methodref found\n");			)
                throwNoSuchMethodError();
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
		class = getClass(ci, this);
		processClass(class, CSTATE_LINKED);

                if (isSpecial == true) {
                        if (!equalUtf8Consts(name, constructor_name) && class !=
 this && instanceof(class, this)) {
                                class = this->superclass;
                        }
                }

DBG(MLOOKUP,	dprintf("getMethodSignatureClass(%s,%s,%s)\n",
			class->name->data, name->data, sig->data);	)

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
}

Hjava_lang_Class*
getClass(constIndex idx, Hjava_lang_Class* this)
{
	constants* pool;
	Utf8Const *name;
	Hjava_lang_Class* class;
	int tag;

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
		lockMutex(this->centry);
		tag = pool->tags[idx];
		name = WORD2UTF(pool->data[idx]);
		unlockMutex(this->centry);

		if (tag == CONSTANT_ResolvedClass) {
			return (CLASS_CLASS(idx, pool));
		}
		break;

	default:
		throwException(ClassFormatError);
		break;
	}

	/* Find the specified class.  We cannot use 'loadClassOrArray' here
	 * because the name is *not* a signature.
	 */
	if (name->data[0] == '[') {
		class = loadArray(name, this->loader);
	}
	else {
		class = loadClass(name, this->loader);
	}
	if (class == 0) {
		throwException(ClassNotFoundException(name->data));
	}

	/* Lock the class while we update the constant pool.  Someone
	 * may have done this already but we don't care.
	 */
	lockMutex(this->centry);
	pool->data[idx] = (ConstSlot)class;
	pool->tags[idx] = CONSTANT_ResolvedClass;
	unlockMutex(this->centry);

	return (class);
}

void
getField(constIndex idx, Hjava_lang_Class* this, bool isStatic, fieldInfo* ret)
{
	constants* pool;
	constIndex ci;
	constIndex ni;
	Field* field;
	Hjava_lang_Class* class;

	pool = CLASS_CONSTANTS(this);
	if (pool->tags[idx] != CONSTANT_Fieldref) {
DBG(FLOOKUP,	dprintf("No Fieldref found\n");				)
                throwException(NoSuchFieldError(""));
	}

	ci = FIELDREF_CLASS(idx, pool);
	class = getClass(ci, this);

	ni = FIELDREF_NAMEANDTYPE(idx, pool);

DBG(FLOOKUP,	dprintf("*** getField(%s,%s,%s)\n",
		class->name->data, 
		WORD2UTF(pool->data[NAMEANDTYPE_NAME(ni, pool)])->data, 
		WORD2UTF(pool->data[NAMEANDTYPE_SIGNATURE(ni, pool)])->data);
    )

	field = lookupClassField(class, WORD2UTF(pool->data[NAMEANDTYPE_NAME(ni, pool)]), isStatic);
	if (field == 0) {
DBG(FLOOKUP,	printf("Field not found\n");				)
                throwException(NoSuchFieldError(WORD2UTF(pool->data[NAMEANDTYPE_NAME(ni, pool)])->data));
	}

	ret->field = field;
	ret->class = class;
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
		if (equalUtf8Consts (name, mptr->name) && equalUtf8Consts (signature, mptr->signature)) {
			if ((mptr->accflags & ACC_ABSTRACT) != 0) {
				SET_METHOD_NATIVECODE(mptr, (void*)throwAbstractMethodError);
				mptr->accflags |= ACC_NATIVE;
			}
			return (mptr);
		}
	}
	return (NULL);
}

/*
 * Lookup a method (and translate) in the specified class.
 */
Method*
findMethod(Hjava_lang_Class* class, Utf8Const* name, Utf8Const* signature)
{
	/*
	 * Waz CSTATE_LINKED - Must resolve constants before we do any
	 * translation.  Might not be right though ... XXX
	 */
	processClass(class, CSTATE_OK);

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
	return (0);
}

#if defined(TRANSLATOR)
/*
 * Find exception in method.
 */
void
findExceptionInMethod(uintp pc, Hjava_lang_Class* class, exceptionInfo* info)
{
	Method* ptr;

	info->handler = 0;
	info->class = 0;
	info->method = 0;

	ptr = findMethodFromPC(pc);
	if (ptr != 0) {
		if (findExceptionBlockInMethod(pc, class, ptr, info) == true) {
			return;
		}
	}
DBG(ELOOKUP,	dprintf("Exception not found.\n");			)
}
#endif

/*
 * Look for exception block in method.
 */
bool
findExceptionBlockInMethod(uintp pc, Hjava_lang_Class* class, Method* ptr, exceptionInfo* info)
{
	jexceptionEntry* eptr;
	Hjava_lang_Class* cptr;
	int i;

	/* Stash method & class */
	info->method = ptr;
	info->class = ptr->class;

	eptr = &ptr->exception_table->entry[0];

DBG(ELOOKUP,	
	dprintf("Nr of exceptions = %d\n", ptr->exception_table->length); )

	/* Right method - look for exception */
	if (ptr->exception_table == 0) {
		return (false);
	}
	for (i = 0; i < ptr->exception_table->length; i++) {
		uintp start_pc = eptr[i].start_pc;
		uintp end_pc = eptr[i].end_pc;
		uintp handler_pc = eptr[i].handler_pc;

DBG(ELOOKUP,	dprintf("Exceptions %x (%x-%x)\n", pc, start_pc, end_pc); )
		if (pc < start_pc || pc > end_pc) {
			continue;
		}
DBG(ELOOKUP,	dprintf("Found exception 0x%x\n", handler_pc); )

		/* Found exception - is it right type */
		if (eptr[i].catch_idx == 0) {
			info->handler = handler_pc;
			return (true);
		}
		/* Resolve catch class if necessary */
		if (eptr[i].catch_type == NULL) {
			eptr[i].catch_type = getClass(eptr[i].catch_idx, ptr->class);
		}
		for (cptr = class; cptr != 0; cptr = cptr->superclass) {
			if (cptr == eptr[i].catch_type) {
				info->handler = handler_pc;
				return (true);
			}
		}
	}
	return (false);
}

static
void
throwNoSuchMethodError(void)
{
	throwException(NoSuchMethodError("method name unknown"));
}

static
void
throwAbstractMethodError(void)
{
	throwException(AbstractMethodError);
}
