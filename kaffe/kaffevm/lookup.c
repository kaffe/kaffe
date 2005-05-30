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
#include "jni_md.h"
#include "errors.h"
#include "gtypes.h"
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
getMethodSignatureClass(constIndex idx, Hjava_lang_Class* this, bool shouldLoadClass, int isSpecial, callInfo* call, errorInfo *einfo)
{
	constants* pool;
	constIndex ci;
	constIndex ni;
	Hjava_lang_Class* class;
	Utf8Const* name;
	Utf8Const* sig;
	Method* mptr;
	int i;

	call->class = NULL;
	call->method = NULL;
	call->signature = NULL;
	call->name = NULL;
	call->cname = NULL;

	pool = CLASS_CONSTANTS(this);
	if (pool->tags[idx] != CONSTANT_Methodref &&
	    pool->tags[idx] != CONSTANT_InterfaceMethodref) {
DBG(RESERROR,	dprintf("No Methodref found for idx=%d\n", idx);	);
		/* shouldn't that be ClassFormatError or something? */
		postExceptionMessage(einfo, JAVA_LANG(NoSuchMethodError),
			"method name unknown, tag = %d", pool->tags[idx]);
                return (false);
	}

	ni = METHODREF_NAMEANDTYPE(idx, pool);
	name = WORD2UTF(pool->data[NAMEANDTYPE_NAME(ni, pool)]);
	sig = WORD2UTF(pool->data[NAMEANDTYPE_SIGNATURE(ni, pool)]);

	call->name = name;
	call->signature = sig;

	if (shouldLoadClass == true) {
		ci = METHODREF_CLASS(idx, pool);
		class = getClass(ci, this, einfo);
		if (class == NULL) {
			call->cname = WORD2UTF(pool->data[ci]);
			countInsAndOuts(sig->data, &call->in, &call->out, &call->rettype);
			return (false);
		}
		assert(class->state >= CSTATE_DOING_LINK);

                if (isSpecial == 1) {
                        if (!utf8ConstEqual(name, constructor_name) &&
			    class != this && instanceof(class, this)) {
                                class = this->superclass;
                        }
                }

		call->class = class;
		call->cname = class->name;
		call->method = NULL;
		/* Find method - we don't use findMethod(...) yet since this
		 * will initialise our class (and we don't want to do that).
		 */
		if (isSpecial != 2) {
			/* method resolution for INVOKEVIRTUAL or INVOKESPECIAL.
			 * Since we make sure that the dispatch table of a class
			 * contains all methods of all implemented interfaces, we
			 * don't need to search superinterfaces here.
			 */
			for (; class != NULL; class = class->superclass) {
				mptr = findMethodLocal(class, name, sig);
				if (mptr != NULL) {
					call->method = mptr;
					break;
				}
			}
		} else {
			/* method resolution for INVOKEINTERFACE. First search the
			 * interface itself, then its superinterfaces and finally
			 * java.lang.Object.
			 */
			class = call->class;
			mptr = findMethodLocal(class, name, sig);
			if (mptr == NULL) {
				for (i = class->total_interface_len - 1; i >= 0; i--) {
					mptr = findMethodLocal(class->interfaces[i], name, sig);
					if (mptr != NULL) {
						break;
					}
				}

				if (mptr == NULL) {
					mptr = findMethodLocal(call->class->superclass, name, sig);
				}
			}
			call->method = mptr;
		}
	}

	/* Calculate in's and out's */
	countInsAndOuts(sig->data, &call->in, &call->out, &call->rettype);

DBG(MLOOKUP,	
	if (shouldLoadClass) 
		dprintf("getMethodSignatureClass(%s,%s,%s) -> %s\n",
			call->class->name->data, name->data, sig->data, 
			(call->method ? "success" : "failure"));	);
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
		lockClass(this);
		tag = pool->tags[idx];
		name = WORD2UTF(pool->data[idx]);
		unlockClass(this);

		if (tag == CONSTANT_ResolvedClass) {
			return (CLASS_CLASS(idx, pool));
		}
		break;

	default:
		postExceptionMessage(einfo,
				     JAVA_LANG(ClassFormatError),
				     "%s (Invalid constant reference, %d, "
				     "expecting class, likely an internal "
				     "error)",
				     this->name->data,
				     tag);
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
	if (class == 0) {
		/*
		 * Need to translate ClassNotFoundExceptions to
		 * NoClassDefFoundErrors since thats what the
		 * verifier/jitter/etc expect.
		 */
		switch( einfo->type & KERR_CODE_MASK )
		{
		case KERR_RETHROW:
			if( soft_instanceof(javaLangClassNotFoundException,
					    (Hjava_lang_Object *)
					    einfo->throwable) )
			{
				discardErrorInfo(einfo);
				postNoClassDefFoundError(einfo, name->data);
			}
			break;
		case KERR_EXCEPTION:
			if( strcmp(einfo->classname,
				   JAVA_LANG(ClassNotFoundException)) == 0 )
			{
				errorInfo einfo_copy = *einfo;
				
				postNoClassDefFoundError(einfo, einfo->mess);
				discardErrorInfo(&einfo_copy);
			}
			break;
		default:
			break;
		}
		return NULL;
	}

	/* Lock the class while we update the constant pool.  Someone
	 * may have done this already but we don't care.
	 */
	lockClass(this);
	pool->data[idx] = (ConstSlot)class;
	pool->tags[idx] = CONSTANT_ResolvedClass;
	unlockClass(this);

	return (class);
}

bool
getField(constIndex idx, Hjava_lang_Class* this, bool isStaticField, fieldInfo* ret, errorInfo *einfo)
{
	constants* pool;
	constIndex ci;
	constIndex ni;
	Field* field;
	Hjava_lang_Class* class;

	ret->field = NULL;
	ret->class = NULL;
	
	pool = CLASS_CONSTANTS(this);
	if (pool->tags[idx] != CONSTANT_Fieldref) {
DBG(RESERROR,	dprintf("No Fieldref found\n");				);
		postExceptionMessage(einfo, JAVA_LANG(NoSuchFieldError),
			"tag was %d", pool->tags[idx]);
		return (false);
	}

	ci = FIELDREF_CLASS(idx, pool);

	ni = FIELDREF_NAMEANDTYPE(idx, pool);

	ret->name = WORD2UTF(pool->data[NAMEANDTYPE_NAME(ni, pool)]);
	ret->signature = WORD2UTF(pool->data[NAMEANDTYPE_SIGNATURE(ni, pool)]);

	class = getClass(ci, this, einfo);
	if (class == NULL) {
		ret->cname = WORD2UTF(pool->data[ci]);
		return (false);
	}

DBG(FLOOKUP,	dprintf("*** getField(%s,%s,%s)\n",
		class->name->data, 
		WORD2UTF(pool->data[NAMEANDTYPE_NAME(ni, pool)])->data, 
		WORD2UTF(pool->data[NAMEANDTYPE_SIGNATURE(ni, pool)])->data);
    );
	ret->cname = class->name;

	field = lookupClassField(class, WORD2UTF(pool->data[NAMEANDTYPE_NAME(ni, pool)]), isStaticField, einfo);
	if (field == 0) {
		return (false);
	}

	if (!utf8ConstEqual(field->signature, ret->signature)) {
		postExceptionMessage(einfo,
				     JAVA_LANG(NoSuchFieldError),
				     "%s.%s %s",
				     ret->cname->data,
				     ret->name->data,
				     ret->signature->data);
		return (false);
	}

	/* XXX Should we verify that ret->signature matches field? */

	ret->field = field;
	ret->class = field->clazz;
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
		if (utf8ConstEqual (name, mptr->name) && utf8ConstEqual (signature, METHOD_SIG(mptr))) {
DBG(MLOOKUP,
			dprintf("findMethodLocal(%s,%s,%s) -> %p\n",
				class->name->data, name->data, signature->data, mptr); );

			return (mptr);
		}
	}

DBG(MLOOKUP,
	dprintf("findMethodLocal(%s,%s,%s) -> NOT FOUND\n",
		class->name->data, name->data, signature->data); );

	return NULL;
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
			return (NULL);
	}

	/*
	 * Lookup method - this could be alot more efficient but never mind.
	 * Also there is no attempt to honour PUBLIC, PRIVATE, etc.
	 */
	for (; class != NULL; class = class->superclass) {
		Method* mptr = findMethodLocal(class, name, signature);
		if (mptr != NULL) {
			return mptr;
		}
	}
	postExceptionMessage(einfo, JAVA_LANG(NoSuchMethodError), "%s", name->data);
	return (NULL);
}
