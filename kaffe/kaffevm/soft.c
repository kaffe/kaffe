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
#include "errors.h"
#include "classMethod.h"
#include "baseClasses.h"
#include "lookup.h"
#include "exception.h"
#include "locks.h"
#include "soft.h"
#include "external.h"
#include "thread.h"
#include "baseClasses.h"
#include "itypes.h"
#include "machine.h"
#include "fp.h"

/*
 * soft_new
 */
void*
soft_new(Hjava_lang_Class* c)
{
	Hjava_lang_Object* obj;
	errorInfo info;

	if (processClass(c, CSTATE_COMPLETE, &info) == false) {
		throwError(&info);
	}
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
		arraydims = checkPtr(KCALLOC(dims+1, sizeof(int)));
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
        arraydims[i] = -1;

        /* Mmm, okay now build the array using the wonders of recursion */
        obj = newMultiArray(class, arraydims);


	if (arraydims != array) {
		KFREE(arraydims);
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
		arraydims = checkPtr(KCALLOC(dims+1, sizeof(int)));
	}

	/* Extract the dimensions into an array */
	va_start(ap, dims);
	for (i = 0; i < dims; i++) {
		arg = va_arg(ap, jint);
		if (arg < 0) {
			va_end(ap);
			if (arraydims != array) {
				KFREE(arraydims);
			}
                        throwException(NegativeArraySizeException);
		}
		arraydims[i] = arg;
	}
	arraydims[i] = -1;
	va_end(ap);

	/* Mmm, okay now build the array using the wonders of recursion */
        obj = newMultiArray(class, arraydims);

	if (arraydims != array) {
		KFREE(arraydims);
	}

	/* Return the base object */
	return (obj);
}
#endif

/*
 * soft_lookupmethod.
 */
void*
soft_lookupmethod(Hjava_lang_Object* obj, Hjava_lang_Class* ifclass, int idx)
{
	Hjava_lang_Class* cls;
	Hjava_lang_Class** c;
	Method* meth;
	void*	ncode;
	int i;
	int dtableidx;
	errorInfo info;

	cls = OBJECT_CLASS(obj);
	/* initialize class if necessary */
        if (cls->state < CSTATE_USABLE) {
		if (processClass(cls, CSTATE_COMPLETE, &info) == false) {
			throwError(&info);
		}
	}

	c = cls->interfaces;
	i = 0;
	while (*c != ifclass) {
		c++;
		/* do this check inside the loop to avoid loading 
		 * total_interface_len from memory if we hit the first one. 
		 */
		if (++i == cls->total_interface_len) {
			goto notfound;
		}
	}

	dtableidx = cls->itable2dtable[cls->if2itable[i] + idx];
	if (dtableidx == -1) {
		goto notfound;
	}

	ncode = *(void**)((char*)cls->dtable 
			+ DTABLE_METHODOFFSET + dtableidx * DTABLE_METHODSIZE);
	return (ncode);

notfound:
	/*
	 * Compilers following the latest version of the JLS emit a 
	 * INVOKEINTERFACE instruction for methods that aren't defined in 
	 * an interface, but inherited from Object.
	 *
	 * In this case, the JVM must
	 * a) check that the object really implements the interface given
	 * b) find and invoke the method on object.
	 *
	 * The best way to jit that would be a checkcast <interface_type>
	 * followed by an INVOKEVIRTUAL.
	 *
	 * For now, we simply detect the case where an object method is called
	 * and find it by hand using its (name, signature). 
	 */
	if (ifclass == ObjectClass) {
		Method* objm = CLASS_METHODS(ifclass) + idx;
		
		meth = findMethod(cls, objm->name, METHOD_SIG(objm), &info);
		if (meth == 0) {
			throwError(&info);
		}
		return (METHOD_NATIVECODE(meth));
	}
	meth = CLASS_METHODS(ifclass) + idx;
	soft_nosuchmethod(cls, meth->name, METHOD_SIG(meth));
	return (0);
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
		Hjava_lang_Throwable* ccexc;
		const char *fromtype = CLASS_CNAME(OBJECT_CLASS(o));
		const char *totype = CLASS_CNAME(c);
		char *format = "can't cast `%s' to `%s'";
		char *buf = checkPtr(KMALLOC(strlen(fromtype) 
			+ strlen(totype) + strlen(format)));
		sprintf(buf, format, fromtype, totype);
		ccexc = ClassCastException(buf);
		KFREE(buf);
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
		/* NB: This will keep the stacktrace that was built
		 * when the exception was constructed.
		 * If you wanted a new stacktrace corresponding to the
		 * site where the exception is thrown, you'd use 
		 * `throwException' instead.  However, this would slow down 
		 * exceptions because two stacktrace have to be constructed.
		 */
		throwExternalException((Hjava_lang_Throwable*)o);
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
 * soft_divzero.
 */
void
soft_divzero(void)
{
	throwException(ArithmeticException);
}

/*
 * soft_stackoverflow.
 */
void
soft_stackoverflow(void)
{
	throwException(unhand(getCurrentThread())->stackOverflowError);
}

/*
 * soft_nosuchmethod.
 */
void            
soft_nosuchmethod(Hjava_lang_Class* c, Utf8Const* n, Utf8Const* s)
{
	char buf[256];

	sprintf(buf, "%.80s.%.80s%.80s", CLASS_CNAME(c), n->data, s->data);
	throwException(NoSuchMethodError(buf));
}

/*
 * soft_nosuchfield.
 */
void
soft_nosuchfield(Utf8Const* c, Utf8Const* n)
{
	char buf[256];

	sprintf(buf, "%.100s.%.100s", c->data, n->data);
	throwException(NoSuchFieldError(buf));
}

/*
 * soft_initialise_class.
 */
void
soft_initialise_class(Hjava_lang_Class* c)
{
	/* We check this outside the processClass to save a subroutine call */
	if (c->state != CSTATE_COMPLETE) {
		errorInfo info;
		if (processClass(c, CSTATE_COMPLETE, &info) == false) {
			throwError(&info);
		}
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
	errorInfo info;
	FIXUP_TRAMPOLINE_INIT;

	/* If this class needs initializing, do it now.  */
	if (meth->class->state < CSTATE_USABLE &&
		processClass(meth->class, CSTATE_COMPLETE, &info) == false) {
		throwError(&info);
	}

	/* Generate code on demand.  */
	if (!METHOD_TRANSLATED(meth)) {
		if (translate(meth, &info) == false) {
			throwError(&info);
		}
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

TDBG(	fprintf(stderr, "Calling %s:%s%s @ 0x%x\n", meth->class->name->data, meth->name->data, METHOD_SIGD(meth), METHOD_NATIVECODE(meth));	)

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
	jint v1bits;
	jint v2bits;

	v1bits = floatToInt(v1);
	v2bits = floatToInt(v2);

        if (FISNAN(v1bits) || FISNAN(v2bits)) {
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
	jint v1bits;
	jint v2bits;

	v1bits = floatToInt(v1);
	v2bits = floatToInt(v2);

        if (FISNAN(v1bits) || FISNAN(v2bits)) {
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
soft_fadd(jfloat v1, jfloat v2)
{
	return floatAdd(v1, v2);
}

jdouble
soft_faddl(jdouble v1, jdouble v2)
{
	return doubleAdd(v1, v2);
}

jfloat
soft_fsub(jfloat v1, jfloat v2)
{
	return floatSubtract(v1, v2);
}

jdouble
soft_fsubl(jdouble v1, jdouble v2)
{
	return doubleSubtract(v1, v2);
}


jfloat
soft_fmul(jfloat v1, jfloat v2)
{
	return floatMultiply(v1, v2);
}

jdouble
soft_fmull(jdouble v1, jdouble v2)
{
	return doubleMultiply(v1, v2);
}

jfloat
soft_fdiv(jfloat v1, jfloat v2)
{
	return floatDivide(v1, v2);
}

jdouble
soft_fdivl(jdouble v1, jdouble v2)
{
	return doubleDivide(v1, v2);
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
	jint vbits;

	vbits = floatToInt(v);
        if (FISNAN(vbits)) {
		return (longToDouble(DNANBITS));
	}
	else {
		return ((jdouble)v);
	}
}

jfloat
soft_cvtdf(jdouble v)
{
	jlong vbits;

	vbits = doubleToLong(v);
        if (DISNAN(vbits)) {
		return (intToFloat(FNANBITS));
	}
	else {
		return ((jfloat)v);
	}
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
        jint vbits;

	vbits = floatToInt(v);
        if (FISNAN(vbits)) {
		return (0);
	}

	if (v < 0.0) {
		v = ceil(v);
	}
	else {
		v = floor(v);
	}
	/* If too small return smallest int */
	if (v < -2147483648.0) {
		return (-2147483647-1);
	}
	/* If too big return biggest int */
	else if (v > 2147483647) {
		return (2147483647);
	}
	else {
		return ((jint)v);
	}
}

jint
soft_cvtdi(jdouble v)
{
        jlong vbits;

	vbits = doubleToLong(v);
        if (DISNAN(vbits)) {
		return (0);
	}

	if (v < 0.0) {
		v = ceil(v);
	}
	else {
		v = floor(v);
	}
	/* If too small return smallest int */
	if (v < -2147483648.0) {
		return (-2147483647-1);
	}
	/* If too big return biggest int */
	else if (v > 2147483647) {
		return (2147483647);
	}
	else {
		return ((jint)v);
	}
}

void
soft_debug1(void* a0, void* a1, void* a2)
{
}

void
soft_debug2(void* a0, void* a1, void* a2)
{
}

void
soft_trace(Method* meth, void* args)
{
}
