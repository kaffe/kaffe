/*
 * gcj-soft.cc
 * Provide GCJ's soft functions.
 *
 * Copyright (c) 1996, 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "gcj.h"
#include <stdarg.h>

#if defined(HAVE_GCJ_SUPPORT)
#define private public
#include <java/lang/Object.h>
#include <java/lang/Class.h>
#include <java/lang/reflect/Method.h>
#include <gcj/field.h>
#include <gcj/method.h>
#include <gcj/javaprims.h>

/* XXX FIXME */
extern "C" {
extern jobject soft_newarray(jint type, jint size);
extern JArray<java::lang::Object *> *soft_anewarray(void *type, jint size);

extern void * soft_checkcast(void *type, java::lang::Object *obj);
extern int soft_instanceof(void *type, java::lang::Object *obj);
extern void soft_checkarraystore(java::lang::Object *array, java::lang::Object *obj);

extern void *getCurrentThread(void);
extern void lockObject(java::lang::Object*);
extern void unlockObject(java::lang::Object*);
}

extern "C" java::lang::Object*
/* ARGUSED */
_Jv_AllocObject(java::lang::Class* type, jint size)
{
	if (type->state != JV_STATE_DONE) {
		_Jv_InitClass(type);
	}

	assert(GCJ2KAFFE(type) != 0);
DBG(GCJMORE, 
	dprintf("Jv_Alloc %s, %d ", kenvGetClassName(GCJ2KAFFE(type)), size);
    )
	struct Hjava_lang_Object *o = kenvCreateObject(GCJ2KAFFE(type), size);
DBG(GCJMORE, 
	dprintf("%p\n", o);
    )
	return ((java::lang::Object*)o);
}

extern "C" void
_Jv_InitClass(java::lang::Class* type)
{
DBG(GCJMORE, dprintf("Jv_InitClass @%p\n", type); )

	if (type->state != JV_STATE_DONE) {
DBG(GCJ, 
		if (type && type->name) { 
		    dprintf("Jv_InitClass @%p %s k@%p\n", type, 
		    	type->name->data, GCJ2KAFFE(type));
		} 
    )
		struct Hjava_lang_Class *kClass = GCJ2KAFFE(type);
		if (!kClass) {
			kClass = kenvFindClassByAddress(type);
			assert(kClass || !!!"Kaffe class not found");
			GCJ2KAFFE(type) = kClass;
		}
		kenvProcessClass(kClass);
		type->state = JV_STATE_DONE;
	}
}

extern "C" jint
_Jv_MonitorEnter(java::lang::Object* obj)
{
DBG(GCJ,dprintf("GCJ: lockObject %p\n", obj); )
	lockObject(obj);
	return (0);
}

extern "C" jint
_Jv_MonitorExit(java::lang::Object* obj)
{
DBG(GCJ,dprintf("GCJ: unlockObject %p\n", obj); )
	unlockObject(obj);
	return (0);
}

extern "C" jobject
_Jv_NewArray(jint type, jint size)
{
	/* We assume that array layout is identical and that the types used
	 * to encode the primitive types (see itypes.h)
	 * are the same as _Jv_NewArray (See libgcj/libjava/prims.cc)
	 */
	return (soft_newarray(type, size));
}

extern "C" JArray<java::lang::Object *> *
/* ARGUSED */	// what is obj?
_Jv_NewObjectArray(jint size, java::lang::Class* type, java::lang::Object *obj)
{
DBG(GCJ,dprintf("GCJ: newObjectArray %p\n", obj); )
	if (type->state != JV_STATE_DONE) {
		_Jv_InitClass(type);
	}
	/* gcj wants us to init the array with obj -- why */
	assert(obj == 0);
	return (soft_anewarray(GCJ2KAFFE(type), size));
}

extern "C" java::lang::Object*
_Jv_NewMultiArray(java::lang::Class* clazz, jint dims, ...)
{
DBG(GCJ,
	dprintf("GCJ: _Jv_NewMultiArray %p k@%p %d...\n", clazz, GCJ2KAFFE(clazz), dims);
    )
	void *obj;
	va_list ap;
	va_start(ap, dims);
	obj = soft_vmultianewarray(GCJ2KAFFE(clazz), dims, ap);
	va_end(ap);
	return ((java::lang::Object*)obj);
}

extern "C" void
_Jv_ThrowBadArrayIndex(void)
{
	kenvThrowBadArrayIndex(/* reserved for index */ -9999);
}

extern "C" char *describeObject (java::lang::Object* obj);

extern "C" void * 
_Jv_CheckCast(java::lang::Class* type, java::lang::Object* obj)
{
	void * rc;

DBG(GCJ, dprintf("%s: %p @%p %p\n", __FUNCTION__, type, GCJ2KAFFE(type), obj);)
	if (type->state != JV_STATE_DONE) {
		_Jv_InitClass(type);
	}
	assert(GCJ2KAFFE(type) != 0);
	rc = soft_checkcast(GCJ2KAFFE(type), obj);
DBG(GCJ, 
	if (rc == 0) {
	    dprintf("%s: %p %s is not of type %s\n", __FUNCTION__,
		    obj, (obj ? describeObject(obj) : "<-null->"),
		    kenvGetClassName(GCJ2KAFFE(type)));
	}
   )
	return (rc);
}

extern "C" jboolean
_Jv_IsInstanceOf(java::lang::Object* obj, java::lang::Class* type)
{
DBG(GCJ, dprintf("%s: %p @%p %p\n", __FUNCTION__, type, GCJ2KAFFE(type), obj);)
	if (type->state != JV_STATE_DONE) {
		_Jv_InitClass(type);
	}
	assert(GCJ2KAFFE(type) != 0);
	return (soft_instanceof(GCJ2KAFFE(type), obj));
}

extern "C" void
_Jv_CheckArrayStore(java::lang::Object* array, java::lang::Object* obj)
{
	soft_checkarraystore(array, obj);
}

/* NB.: gcj will pass the run-time type of the object as the first parameter,
 * which is a kaffe class.  See also comment in kenvFindMethod.
 */
extern "C" void*
_Jv_LookupInterfaceMethod(struct Hjava_lang_Class* kclass, _Jv_Utf8Const* mname, _Jv_Utf8Const* msig)
{
	/* class must have been initialized */
    	assert(kclass != 0);

DBG(GCJ,
	dprintf("%s: %s.%s.%s\n", __FUNCTION__, 
		kenvGetClassName(kclass), mname->data, msig->data);
    )
    	return (kenvFindMethod(kclass, mname->data, msig->data));
}

#endif
