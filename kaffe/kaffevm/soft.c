/*
 * soft.c
 * Soft instruction support.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "debug.h"
#define	TDBG(s)

#include "config.h"
#include "config-std.h"
#include "config-math.h"
#include "config-mem.h"
#include <stdarg.h>
#include "gtypes.h"
#include "bytecode.h"
#include "slots.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "classMethod.h"
#include "baseClasses.h"
#include "lookup.h"
#include "errors.h"
#include "exception.h"
#include "locks.h"
#include "soft.h"
#include "external.h"
#include "thread.h"
#include "baseClasses.h"
#include "itypes.h"
#include "machine.h"

/*
 * soft_new
 */
void*
soft_new(Hjava_lang_Class* c)
{
	Hjava_lang_Object* obj;

	processClass(c, CSTATE_OK);
	obj = newObject(c);

DBG(NEWINSTR,	
	dprintf("New object of type %s (%d,%x)\n", 
		c->name->data, c->bfsize, obj); )

	return (obj);
}

/*
 * soft_newarray
 */
void*
soft_newarray(jint type, jint size)
{
	Hjava_lang_Object* obj;

	if (size < 0) {
		throwException(NegativeArraySizeException);
	}

	obj = newArray(TYPE_CLASS(type), size);

DBG(NEWINSTR,	
	dprintf("New array of %s [%d] (%x)\n", 
		TYPE_CLASS(type)->name->data, size, obj); )

	return (obj);
}

/*
 * soft_anewarray
 */
void*
soft_anewarray(Hjava_lang_Class* elclass, jint size)
{
	Hjava_lang_Object* obj;

	if (size < 0) {
		throwException(NegativeArraySizeException);
	}

	obj = newArray(elclass, size);

DBG(NEWINSTR,	
	dprintf("New array object [%d] of %s (%x)\n", size,
		elclass->name->data, obj); )
	return (obj);
}

/*
 * soft_multianewarray.
 */
#define	MAXDIMS	16

#if defined(INTERPRETER)
void*
soft_multianewarray(Hjava_lang_Class* class, jint dims, slots* args)
{
        int array[MAXDIMS];
        Hjava_lang_Object* obj;
        jint arg;
	int i;
	int* arraydims;

        if (dims < MAXDIMS) {
		arraydims = array;
	}
	else {
		arraydims = gc_calloc_fixed(dims+1, sizeof(int));
	}

	/* stack grows up, so move to the first dimension */
	args -= dims-1;

        /* Extract the dimensions into an array */
        for (i = 0; i < dims; i++) {
		arg = args[i].v.tint;
                if (arg < 0) {
                        throwException(NegativeArraySizeException);
		}
                arraydims[i] = arg;
        }
        arraydims[i] = 0;

        /* Mmm, okay now build the array using the wonders of recursion */
        obj = newMultiArray(class, arraydims);


	if (arraydims != array) {
		gc_free_fixed(arraydims);
	}

        /* Return the base object */
	return (obj);
}
#endif

#if defined(TRANSLATOR)
void*
soft_multianewarray(Hjava_lang_Class* class, jint dims, ...)
{
	va_list ap;
	int array[MAXDIMS];
	int i;
	jint arg;
	Hjava_lang_Object* obj;
	int* arraydims;

        if (dims < MAXDIMS) {
		arraydims = array;
	}
	else {
		arraydims = gc_calloc_fixed(dims+1, sizeof(int));
	}

	/* Extract the dimensions into an array */
	va_start(ap, dims);
	for (i = 0; i < dims; i++) {
		arg = va_arg(ap, jint);
		if (arg < 0) {
                        throwException(NegativeArraySizeException);
		}
		arraydims[i] = arg;
	}
	arraydims[i] = 0;
	va_end(ap);

	/* Mmm, okay now build the array using the wonders of recursion */
        obj = newMultiArray(class, arraydims);

	if (arraydims != array) {
		gc_free_fixed(arraydims);
	}

	/* Return the base object */
	return (obj);
}
#endif

/*
 * soft_lookupmethod.
 */
void*
soft_lookupmethod(Hjava_lang_Object* obj, Utf8Const* name, Utf8Const* sig)
{
	Hjava_lang_Class* cls;
	Method* meth;

	cls = OBJECT_CLASS(obj);
	meth = findMethod(cls, name, sig);
	if (meth == 0) {
		throwException(NoSuchMethodError(name->data));
	}

#if defined(TRANSLATOR)
	return (METHOD_NATIVECODE(meth));
#else
	return (meth);
#endif
}

#if 0
/*
 * soft_lookupinterfacemethod
 * Lookup an interface method, given its name and signature.  Throw exceptions
 * if the method found doesn't fit with wht invokeinterface is expecting.
 */
void*
soft_lookupinterfacemethod(Hjava_lang_Class* class, Utf8Const* name, Utf8Const* signature)
{
	processClass(class, CSTATE_LINKED);
	for (; class != 0; class = class->superclass) {
		Method* mptr = findMethodLocal(class, name, signature);
		if (mptr != NULL) {
			if (mptr->accflags & ACC_STATIC) {
				throwException(IncompatibleClassChangeError);
			}
			if (mptr->accflags & ACC_ABSTRACT) {
				throwException(AbstractMethodError);
			}
			if (!(mptr->accflags & ACC_PUBLIC)) {
				throwException(IllegalAccessError);
			}
#if defined(TRANSLATOR)
			return (METHOD_NATIVECODE(mptr));
#else
			return (mptr);
#endif
		}
	}
	throwException(IncompatibleClassChangeError);
}
#endif

inline
jint
instanceof_class(Hjava_lang_Class* c, Hjava_lang_Class* oc)
{
	Hjava_lang_Class* tc;

	/* Check for superclass matches */
	for (tc = oc->superclass; tc != 0; tc = tc->superclass) {
		if (c == tc) {
			return (1);
		}
	}
	return (0);
}

inline
jint
instanceof_interface(Hjava_lang_Class* c, Hjava_lang_Class* oc)
{
	int i;

	/* Check 'total' interface list */
	for (i = oc->total_interface_len - 1; i >= 0; i--) {
		if (c == oc->interfaces[i]) {
			return (1);
		}
	}
	return (0);
}

inline
jint
instanceof_array(Hjava_lang_Class* c, Hjava_lang_Class* oc)
{
	/* Skip as many arrays of arrays as we can.  We stop when we find
	 * a base class in either. */
	while (CLASS_IS_ARRAY(c) && CLASS_IS_ARRAY(oc)) {
		c = CLASS_ELEMENT_TYPE(c);
		oc = CLASS_ELEMENT_TYPE(oc);
	}

	/* If we are still casting to an array then we have failed already */
	if (CLASS_IS_ARRAY(c))
		return (0);

	/* If a base type, they must match exact. */
	if (CLASS_IS_PRIMITIVE(c)) {
		return (c == oc);
	}

	/* Casting to an object of some description. */
	if (CLASS_IS_ARRAY(oc)) {
		/* The only thing we can cast an array to is java/lang/Object.
		 * Checking this here willl save time.
		 */
		return (c == ObjectClass);
	}

	/* Cannot cast to a primitive class. */
	if (CLASS_IS_PRIMITIVE(oc)) {
		return (0);
	}

	/* Casting one object to another.  */
	return (instanceof(c, oc));
}

jint
instanceof(Hjava_lang_Class* c, Hjava_lang_Class* oc)
{
	/* Handle the simplest case first - they are the same */
	if (c == oc) {
		return (1);
	}
	/* Else if an array check that */
	else if (CLASS_IS_ARRAY(c)) {
		return (instanceof_array(c, oc));
	}
	/* Else if an interface check that */
	else if (CLASS_IS_INTERFACE(c)) {
		return (instanceof_interface(c, oc));
	}
	/* Else is must to a class */
	else {
		return (instanceof_class(c, oc));
	}
}

/*
 * soft_instanceof.
 */
jint
soft_instanceof(Hjava_lang_Class* c, Hjava_lang_Object* o)
{
	/* Null object are never instances of anything */
	if (o == 0) {
		return (0);
	}

	return (instanceof(c, OBJECT_CLASS(o)));
}

/*
 * soft_checkcast.
 */
void*
soft_checkcast(Hjava_lang_Class* c, Hjava_lang_Object* o)
{
	if (o != 0 && !instanceof(c, OBJECT_CLASS(o))) {
		/* 
		 * Let's be a bit more informative as to why the class 
		 * cast exception happened.
		 */
		Hjava_lang_Object* ccexc;
		char *fromtype = CLASS_CNAME(OBJECT_CLASS(o));
		char *totype = CLASS_CNAME(c);
		char *format = "can't cast `%s' to `%s'";
		char *buf = gc_malloc_fixed(strlen(fromtype) 
			+ strlen(totype) + strlen(format));
		sprintf(buf, format, fromtype, totype);
		ccexc = ClassCastException(buf);
		gc_free_fixed(buf);
		throwException(ccexc);
	}
	return (o);
}


/*
 * soft_athrow.
 */
void
soft_athrow(Hjava_lang_Object* o)
{
	if (o == 0) {
		soft_nullpointer();
	}
	else {
		throwExternalException(o);
	}
}

/*
 * soft_badarrayindex.
 */
void
soft_badarrayindex(void)
{
	throwException(ArrayIndexOutOfBoundsException);
}

/*
 * soft_nullpointer.
 */
void
soft_nullpointer(void)
{
	throwException(NullPointerException);
}

/*
 * soft_nosuchmethod.
 */
void            
soft_nosuchmethod(Hjava_lang_Class* c, Utf8Const* n, Utf8Const* s)
{
	char buf[100];
	sprintf(buf, "%s.%s%s", CLASS_CNAME(c), n->data, s->data);
	throwException(NoSuchMethodError(buf));
}

/*
 * soft_initialise_class.
 */
void
soft_initialise_class(Hjava_lang_Class* c)
{
	/* We check this outside the processClass to save a subroutine call */
	if (c->state != CSTATE_OK) {
		processClass(c, CSTATE_OK);
	}
}

#if defined(TRANSLATOR)
/*
 * Trampolines come in here - do the translation and replace the trampoline.
 */
nativecode*
soft_fixup_trampoline(FIXUP_TRAMPOLINE_DECL)
{
	Method* meth;
	FIXUP_TRAMPOLINE_INIT;

	/* If this class needs initializing, do it now.  */
	processClass(meth->class, CSTATE_OK);

	/* Generate code on demand.  */
	if (!METHOD_TRANSLATED(meth)) {
		translate(meth);
	}

	/* Update dispatch table */
	if (meth->idx >= 0) {
		meth->class->dtable->method[meth->idx] = METHOD_NATIVECODE(meth);
	}

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

TDBG(	fprintf(stderr, "Calling %s:%s%s @ 0x%x\n", meth->class->name->data, meth->name->data, meth->signature->data, METHOD_NATIVECODE(meth));	)

	return (METHOD_NATIVECODE(meth));
}
#endif

/*
 * Check we can store 'obj' into the 'array'.
 */
void
soft_checkarraystore(Hjava_lang_Object* array, Hjava_lang_Object* obj)
{
	if (obj != 0 && soft_instanceof(CLASS_ELEMENT_TYPE(OBJECT_CLASS(array)), obj) == 0) {
		throwException(ArrayStoreException);
	}
}

/*
 * soft_dcmpg
 */
jint
soft_dcmpg(jdouble v1, jdouble v2)
{
	jint ret;
	if ((!isinf(v1) && isnan(v1)) || (!isinf(v2) && isnan(v2))) {
		ret = 1;
	}
	else if (v1 > v2) {
		ret = 1;
	}
	else if (v1 == v2) {
		ret = 0;
	}
	else {
		ret = -1;
	}

	return (ret);
}

/*
 * soft_dcmpl
 */
jint
soft_dcmpl(jdouble v1, jdouble v2)
{
        jint ret;
	if ((!isinf(v1) && isnan(v1)) || (!isinf(v2) && isnan(v2))) {
		ret = -1;
	}
        else if (v1 > v2) {
                ret = 1;
        }
        else if (v1 == v2) {
                ret = 0;
        }
        else {
                ret = -1;
        }
	return (ret);
}

/*
 * soft_fcmpg
 */
jint
soft_fcmpg(jfloat v1, jfloat v2)
{
        jint ret;
	if ((!isinf(v1) && isnan(v1)) || (!isinf(v2) && isnan(v2))) {
		ret = 1;
	}
        else if (v1 > v2) {
                ret = 1;
        }
        else if (v1 == v2) {
                ret = 0;
        }
        else {
                ret = -1;
        }
	return (ret);
}

/*
 * soft_fcmpg
 */
jint
soft_fcmpl(jfloat v1, jfloat v2)
{
        jint ret;
	if ((!isinf(v1) && isnan(v1)) || (!isinf(v2) && isnan(v2))) {
		ret = -1;
	}
        else if (v1 > v2) {
                ret = 1;
        }
        else if (v1 == v2) {
                ret = 0;
        }
        else {
                ret = -1;
        }
	return (ret);
}

jlong
soft_lmul(jlong v1, jlong v2)
{
	return (v1 * v2);
}

jlong
soft_ldiv(jlong v1, jlong v2)
{
	return (v1 / v2);
}

jlong
soft_lrem(jlong v1, jlong v2)
{
	return (v1 % v2);
}

jfloat
soft_fdiv(jfloat v1, jfloat v2)
{
	jfloat k = 1e300;

	if (v2 == 0.0) {
		if (v1 > 0) {
			return (k*k);
		}
		else {
			return (-k*k);
		}
	}
	return (v1 / v2);
}

jdouble
soft_fdivl(jdouble v1, jdouble v2)
{
	jdouble k = 1e300;

	if (v2 == 0.0) {
		if (v1 > 0) {
			return (k*k);
		}
		else {
			return (-k*k);
		}
	}
	return (v1 / v2);
}

jfloat
soft_frem(jfloat v1, jfloat v2)
{
	return (javaRemainderf(v1, v2));
}

jdouble
soft_freml(jdouble v1, jdouble v2)
{
	return (javaRemainder(v1, v2));
}

jlong
soft_lshll(jlong v1, jint v2)
{
	return (v1 << (v2 & 63));
}

jlong
soft_ashrl(jlong v1, jint v2)
{
	return (v1 >> (v2 & 63));
}

jlong
soft_lshrl(jlong v1, jint v2)
{
	return (((uint64)v1) >> (v2 & 63));
}

jint
soft_lcmp(jlong v1, jlong v2)
{
#if 0
	jlong lcc = v2 - v1;
	if (lcc < 0) {
		return (-1);
	}
	else if (lcc > 0) {
		return (1);
	}
	else {
		return (0);
	}
#endif
	if (v2 < v1) {
		return (-1);
	}
	else if (v2 > v1) {
		return (1);
	}
	else {
		return (0);
	}
}

jint
soft_mul(jint v1, jint v2)
{
	return (v1*v2);
}

jint
soft_div(jint v1, jint v2)
{
	return (v1/v2);
}

jint
soft_rem(jint v1, jint v2)
{
	return (v1%v2);
}

jfloat
soft_cvtlf(jlong v)
{
	return ((jfloat)v);
}

jfloat
soft_cvtif(jint v)
{
	return ((jfloat)v);
}

jdouble
soft_cvtid(jint v)
{
	return ((jdouble)v);
}

jdouble
soft_cvtld(jlong v)
{
	return ((jdouble)v);
}

jdouble
soft_cvtfd(jfloat v)
{
	return ((jdouble)v);
}

jfloat
soft_cvtdf(jdouble v)
{
	return ((jfloat)v);
}

/*
 * The following functions round the float/double to an int/long.
 * They round the value toward zero.
 */

jlong
soft_cvtfl(jfloat v)
{
	if (v < 0.0) {
		return ((jlong)ceil(v));
	}
	else {
		return ((jlong)floor(v));
	}
}

jlong
soft_cvtdl(jdouble v)
{
	if (v < 0.0) {
		return ((jlong)ceil(v));
	}
	else {
		return ((jlong)floor(v));
	}
}

jint
soft_cvtfi(jfloat v)
{
	if (v < 0.0) {
		return ((jint)ceil(v));
	}
	else {
		return ((jint)floor(v));
	}
}

jint
soft_cvtdi(jdouble v)
{
	if (v < 0.0) {
		return ((jint)ceil(v));
	}
	else {
		return ((jint)floor(v));
	}
}
