/*
 * gcj-soft.c
 * Provide GCJ's soft functions.
 *
 * Copyright (c) 1996, 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "config-std.h"
#include "classMethod.h"
#include "errors.h"
#include "soft.h"
#include "lookup.h"
#include "locks.h"
#include "gc.h"
#include "exception.h"
#include "stringSupport.h"
#include "gcj.h"

#if defined(HAVE_GCJ_SUPPORT)

Hjava_lang_Object*
_Jv_AllocObject(Hjava_lang_Class* type)
{
	return (soft_new(type));
}

void
_Jv_InitClass(Hjava_lang_Class* type)
{
	errorInfo err;

	if (type->state != CSTATE_COMPLETE) {
		processClass(type, CSTATE_COMPLETE, &err);
	}
}

void
_Jv_Throw(Hjava_lang_Object* obj)
{
	soft_athrow(obj);
}

void
_Jv_MonitorEnter(Hjava_lang_Object* obj)
{
	lockMutex(obj);
}

void
_Jv_MonitorExit(Hjava_lang_Object* obj)
{
	unlockMutex(obj);
}

Hjava_lang_Object*
_Jv_NewArray(jint type, jint size)
{
	return (soft_newarray(type, size));
}

Hjava_lang_Object*
_Jv_NewObjectArray(jint size, Hjava_lang_Class* type)
{
	return (soft_anewarray(type, size));
}

Hjava_lang_Object*
_Jv_NewMultiArray(Hjava_lang_Class* class, jint dims, ...)
{
        int array[16];
        Hjava_lang_Object* obj;
        jint arg;
        int i;
        int* arraydims;
        va_list ap;

        if (dims < 16) {
                arraydims = array;
        }
        else {
                arraydims = jmalloc((dims+1) * sizeof(int));
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
                jfree(arraydims);
        }

        /* Return the base object */
        return (obj);
}

void
_Jv_ThrowBadArrayIndex(void)
{
	soft_badarrayindex();
}

Hjava_lang_Object*
_Jv_CheckCast(Hjava_lang_Class* type, Hjava_lang_Object* obj)
{
	return (soft_checkcast(type, obj));
}

jboolean
_Jv_IsInstanceOf(Hjava_lang_Object* obj, Hjava_lang_Class* type)
{
	return (soft_instanceof(type, obj));
}

void
_Jv_CheckArrayStore(Hjava_lang_Object* array, Hjava_lang_Object* obj)
{
	soft_checkarraystore(array, obj);
}

void*
_Jv_LookupInterfaceMethod(Hjava_lang_Class* objcls, Utf8Const* mname, Utf8Const* msig)
{
	Method* meth;
	errorInfo err;

	meth = findMethod(objcls, mname, msig, &err);
	if (meth == 0) {
		throwException(NoSuchMethodError(mname->data));
	}
	return (METHOD_NATIVECODE(meth));
}

#endif
