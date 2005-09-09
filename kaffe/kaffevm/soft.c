/*
 * soft.c
 * Soft instruction support.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *	Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *   
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include <stdarg.h>

#include "debug.h"
#include "config.h"
#include "config-std.h"
#include "config-math.h"
#include "config-mem.h"
#include "gtypes.h"
#include "kaffe/jmalloc.h"
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
#include "jvmpi_kaffe.h"

void
soft_null_call(void)
{
}

/*
 * soft_new
 */
void*
soft_new(Hjava_lang_Class* c)
{
	Hjava_lang_Object* obj;
	errorInfo info;

	if (c->state != CSTATE_COMPLETE && processClass(c, CSTATE_COMPLETE, &info) == false) {
		goto bad;
	}
	obj = newObjectChecked(c, &info);
	if (obj == NULL) {
		goto bad;
	}

DBG(NEWINSTR,
	dprintf("New object of type %s (%d,%p)\n",
		c->name->data, CLASS_FSIZE(c), obj); );

	return (obj);
bad:
	throwError(&info);
	return (NULL);
}

/*
 * soft_newarray
 */
void*
soft_newarray(jint type, jint size)
{
	Hjava_lang_Object* obj;
	errorInfo info;

	if (size < 0) {
		throwException(NegativeArraySizeException);
	}

	obj = newArrayChecked(TYPE_CLASS(type), (jsize)size, &info);
	if (obj == NULL) {
		throwError(&info);
	}

DBG(NEWINSTR,
	dprintf("New array of %s [%d] (%p)\n",
		TYPE_CLASS(type)->name->data, size, obj); );

	return (obj);
}

/*
 * soft_anewarray
 */
void*
soft_anewarray(Hjava_lang_Class* elclass, jint size)
{
	Hjava_lang_Object* obj;
	errorInfo info;

	if (size < 0) {
		throwException(NegativeArraySizeException);
	}

	obj = newArrayChecked(elclass, (jsize)size, &info);
	if (obj == NULL) {
		throwError(&info);
	}

DBG(NEWINSTR,
	dprintf("New array object [%d] of %s (%p)\n", size,
		elclass->name->data, obj); );
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
	errorInfo einfo;
        int array[MAXDIMS];
        Hjava_lang_Object* obj;
        jint arg;
	int i;
	int* arraydims;

        if (dims < MAXDIMS-1) {
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
        obj = newMultiArrayChecked(class, arraydims, &einfo);

	if (arraydims != array) {
		KFREE(arraydims);
	}

	if (!obj) {
		throwError(&einfo);
	}

        /* Return the base object */
	return (obj);
}
#endif

#if defined(TRANSLATOR)
void*
soft_vmultianewarray(Hjava_lang_Class* class, jint dims, va_list ap)
{
	errorInfo einfo;
	int array[MAXDIMS];
	int i;
	jint arg;
	Hjava_lang_Object* obj;
	int* arraydims;

        if (dims < MAXDIMS-1) {
		arraydims = array;
	}
	else {
		arraydims = checkPtr(KCALLOC(dims+1, sizeof(int)));
	}

	/* Extract the dimensions into an array */
	for (i = 0; i < dims; i++) {
		arg = va_arg(ap, jint);
		if (arg < 0) {
			if (arraydims != array) {
				KFREE(arraydims);
			}
                        throwException(NegativeArraySizeException);
		}
		arraydims[i] = arg;
	}
	arraydims[i] = -1;

	/* Mmm, okay now build the array using the wonders of recursion */
        obj = newMultiArrayChecked(class, arraydims, &einfo);

	if (arraydims != array) {
		KFREE(arraydims);
	}

	if (!obj) {
		throwError(&einfo);
	}

	/* Return the base object */
	return (obj);
}

void*
soft_multianewarray(Hjava_lang_Class* class, jint dims, ...)
{
	void* obj;
	va_list ap;
	va_start(ap, dims);
	obj = soft_vmultianewarray(class, dims, ap);
	va_end(ap);
	return (obj);
}
#endif

/*
 * soft_lookupinterfacemethod.
 */
void*
soft_lookupinterfacemethod(Hjava_lang_Object* obj, Hjava_lang_Class* ifclass, int idx)
{
	Hjava_lang_Class* cls;
	void*	ncode;
	register int i;
	register void*** implementors;

	if (obj == NULL) {
		soft_nullpointer();
	}

	cls = OBJECT_CLASS(obj);
	implementors = ifclass->implementors;
	i = cls->impl_index;

#if 1	/* it should never be necessary to initialize the class here
	 * cause we're invoking a method on an existing, created object
	 */
        assert (cls->state >= CSTATE_USABLE);
#else
	/* initialize class if necessary */
        if (cls->state < CSTATE_USABLE) {
		errorInfo info;
		if (processClass(cls, CSTATE_COMPLETE, &info) == false) {
			throwError(&info);
		}
	}
#endif
	/* skip word at the beginning of itable2dtable */
	ncode = implementors[i][idx + 1];

	/* This means this class does not implement this interface method
	 * at all.  This is something we detect at the time the interface
	 * dispatch table is built.  To avoid this test, we could instead
	 * point at a nosuchmethod routine there.  However, we would have
	 * to find a way to pass along the information which methods it
	 * is that's missing (or create multiple nosuch_method routines,
	 * given that they should be rare---minus possible DoS.)
	 */
	if (ncode == (void *)-1)
	  return NULL;
	assert(ncode != NULL);

	return ncode;
}

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
	unsigned int i;
	Hjava_lang_Class **impl_clazz;

	if (oc->state < CSTATE_PREPARED || c->state < CSTATE_PREPARED || CLASS_IS_ARRAY(oc) || CLASS_IS_INTERFACE(oc))
	  {
	    /* Check 'total' interface list. If the class is not
	     * prepared the dumb way is the only way. Arrays and interface do not have
	     * any implementors too so we have to go through the all list.
	     */
	    for (i = 0; i < oc->total_interface_len; i++) {
	      if (c == oc->interfaces[i]) {
		return 1;
	      }
	    }
	    return 0;
	  }
	else
	  {
	    /* Fetch the implementation reference from the class. */
	    i = oc->impl_index;
	    /* No interface implemented or this class is not implementing this
	     * interface. Bailing out. */
	    if (i == 0 || c->implementors == NULL ||
		i > (uintp)c->implementors[0] ||
		c->implementors[i] == NULL)
	      return 0;
	    
	    /* We retrieve the first pointer in the itable2dtable array. */
	    impl_clazz = (Hjava_lang_Class **)(KGC_getObjectBase(main_collector, c->implementors[i]));
	    assert(impl_clazz != NULL);
	    
	    /* Now we may compare the raw pointers. */
	    return (*impl_clazz == oc);
	  }
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
	if (o != NULL && !instanceof(c, OBJECT_CLASS(o))) {
		/*
		 * Let's be a bit more informative as to why the class
		 * cast exception happened.
		 */
		Hjava_lang_Throwable* ccexc;
		const char *fromtype = CLASS_CNAME(OBJECT_CLASS(o));
		const char *totype = CLASS_CNAME(c);
		char *buf;
		
		if (c->loader != OBJECT_CLASS(o)->loader) {
			const char *toloader = c->loader?CLASS_CNAME(OBJECT_CLASS((Hjava_lang_Object *)c->loader)):"bootstrap";
			const char *fromloader = OBJECT_CLASS(o)->loader?CLASS_CNAME(OBJECT_CLASS((Hjava_lang_Object *)OBJECT_CLASS(o)->loader)):"bootstrap";
#define _FORMAT "can't cast `%s' (%s@%p) to `%s' (%s@%p)"
			buf = checkPtr(KMALLOC(strlen(fromtype) + 12 + strlen(fromloader)+
				+ strlen(totype) + 12 + strlen(toloader) + strlen(_FORMAT)));
			sprintf(buf, _FORMAT, fromtype, fromloader, OBJECT_CLASS(o)->loader, totype, toloader, c->loader);
#undef _FORMAT
		} else {
#define _FORMAT "can't cast `%s' to `%s'"
			buf = checkPtr(KMALLOC(strlen(fromtype)+strlen(totype)+strlen(_FORMAT)));
			sprintf(buf, _FORMAT, fromtype, totype);
#undef _FORMAT
		}

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
	if (o == NULL) {
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
	Hjava_lang_Throwable *th;
	errorInfo einfo;
	
	/* XXX Dear lord this sucks! */
	KTHREAD(relaxstack)(1);
	th = (Hjava_lang_Throwable *)
		newObjectChecked(javaLangStackOverflowError, &einfo);
	KTHREAD(relaxstack)(0);
	throwException(th);
}

/*
 * soft_nosuchclass.
 */
void
soft_nosuchclass(Utf8Const* c)
{
	char buf[256];

	sprintf(buf, "%.80s", c->data);
	throwException(NoClassDefFoundError(buf));
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

void
soft_linkage(Utf8Const *c, Utf8Const* n)
{
	char buf[256];

	sprintf(buf, "%.100s.%.100s", c->data, n->data);
	throwException(LinkageError(buf));
}

void
soft_illegalaccess(Utf8Const *c, Utf8Const* n)
{
	char buf[256];

	sprintf(buf, "%.100s.%.100s", c->data, n->data);
	throwException(IllegalAccessError(buf));
}

/*
 * soft_incompatibleclasschange.
 */
void
soft_incompatibleclasschange(Utf8Const *c, Utf8Const* n)
{
	char buf[256];

	sprintf(buf, "%.100s.%.100s", c->data, n->data);
	throwException(IncompatibleClassChangeError(buf));
}

/*
 * soft_abstractmethod.
 */
void
soft_abstractmethod(Utf8Const *c, Utf8Const* n)
{
	char buf[256];

	sprintf(buf, "%.100s.%.100s", c->data, n->data);
	throwException(AbstractMethodError(buf));
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


/*
 * Check we can store 'obj' into the 'array'.
 */
void
soft_checkarraystore(Hjava_lang_Object* array, Hjava_lang_Object* obj)
{
	if (obj != NULL && soft_instanceof(CLASS_ELEMENT_TYPE(OBJECT_CLASS(array)), obj) == 0) {
		Hjava_lang_Throwable* asexc;
		const char *otype = CLASS_CNAME(OBJECT_CLASS(obj));
		const char *atype = CLASS_CNAME(OBJECT_CLASS(array));
		char *b;
#define _FORMAT "can't store `%s' in `%s'"
		b = checkPtr(KMALLOC(strlen(otype)+strlen(atype)+strlen(_FORMAT)));
		sprintf(b, _FORMAT, otype, atype);
#undef _FORMAT
		asexc = ArrayStoreException(b);
		KFREE(b);
		throwException(asexc);
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

#if defined(TRANSLATOR)
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
#endif

jint
soft_mul(jint v1, jint v2)
{
	return (v1*v2);
}

jint
soft_div(jint v1, jint v2)
{
	if (v2 == -1)
		return -v1;
	return (v1/v2);
}

jint
soft_rem(jint v1, jint v2)
{
	if (v2 == -1)
		return 0;
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
	jint vbits;

	vbits = floatToInt(v);
        if (FISNAN(vbits)) {
		return ((jlong)0);
	}

	if (v < 0.0) {
		v = ceil(v);
	}
	else {
		v = floor(v);
	}
	/* If too small return smallest long */
	if (v <= -9223372036854775808.0) {
	    return ((jlong)1) << 63;
	}
	/* If too big return biggest long */
	else if (v >= 9223372036854775807.0) {
	    return ~(((jlong)1) << 63);
	}
	else {
	    return ((jlong)v);
	}
}

jlong
soft_cvtdl(jdouble v)
{
	jlong vbits;

	vbits = doubleToLong(v);
        if (DISNAN(vbits)) {
		return ((jlong)0);
	}

	if (v < 0.0) {
		v = ceil(v);
	}
	else {
		v = floor(v);
	}
	/* If too small return smallest long */
	if (v <= -9223372036854775808.0) {
	    return ((jlong)1) << 63;
	}
	/* If too big return biggest long */
	else if (v >= 9223372036854775807.0) {
	    return ~(((jlong)1) << 63);
	}
	else {
	    return ((jlong)v);
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
	if (v <= -2147483648.0f) {
		return (-2147483647-1);
	}

	/* If too big return biggest int */
	if (v >= 2147483647.0f) {
		return (2147483647);
	}

	return ((jint)v);
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
	if (v <= -2147483648.0) {
		return (-2147483647-1);
	}
	/* If too big return biggest int */
	else if (v >= 2147483647.0) {
		return (2147483647);
	}
	else {
		return ((jint)v);
	}
}

void
soft_debug1(void* a0 UNUSED, void* a1 UNUSED, void* a2 UNUSED)
{
}

void
soft_debug2(void* a0 UNUSED, void* a1 UNUSED, void* a2 UNUSED)
{
}

void
soft_trace(Method* meth, void* args UNUSED)
{
    dprintf("soft_trace: %s.%s%s\n", CLASS_CNAME(meth->class), meth->name->data, METHOD_SIGD(meth));
}

#if defined(ENABLE_JVMPI)
void
soft_enter_method(Hjava_lang_Object *obj, Method *meth)
{
	if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_METHOD_ENTRY) )
	{
		JVMPI_Event ev;

		ev.event_type = JVMPI_EVENT_METHOD_ENTRY;
		ev.u.method.method_id = meth;
		jvmpiPostEvent(&ev);
	}
	if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_METHOD_ENTRY2) )
	{
		JVMPI_Event ev;

		ev.event_type = JVMPI_EVENT_METHOD_ENTRY2;
		ev.u.method_entry2.method_id = meth;
		ev.u.method_entry2.obj_id = obj;
		jvmpiPostEvent(&ev);
	}
}
#else
void
soft_enter_method(Hjava_lang_Object *obj UNUSED, Method *meth UNUSED)
{
}
#endif

#if defined(ENABLE_JVMPI)
void
soft_exit_method(Method *meth)
{
	if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_METHOD_EXIT) )
	{
		JVMPI_Event ev;

		ev.event_type = JVMPI_EVENT_METHOD_EXIT;
		ev.u.method.method_id = meth;
		jvmpiPostEvent(&ev);
	}
}
#else
void
soft_exit_method(Method *meth UNUSED)
{
}
#endif
