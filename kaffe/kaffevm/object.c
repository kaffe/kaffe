/*
 * object.c
 * Handle create and subsequent garbage collection of objects.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define	DBG(s)
#define	ADBG(s)

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "classMethod.h"
#include "lookup.h"
#include "itypes.h"
#include "baseClasses.h"
#include "errors.h"
#include "exception.h"
#include "itypes.h"
#include "md.h"
#include "external.h"
#include "gc.h"

extern Hjava_lang_Class* ThreadClass;

extern gcFuncs gcFinalizeObject;
extern gcFuncs gcNormalObject;
extern gcFuncs gcRefArray;
extern gcFuncs gcClassObject;

/*
 * Create a new Java object.
 * If the object is a Class then we make it a root (for the moment).
 * Otherwise, if the object has a finalize method we look it up and store
 * it with the object for it's later use.  Otherwise the object is normal.
 */
Hjava_lang_Object*
newObject(Hjava_lang_Class* class)
{
	Hjava_lang_Object* obj;

	obj = gc_malloc(CLASS_FSIZE(class), GC_ALLOC_NORMALOBJECT);

        /* Fill in object information */
        obj->dtable = class->dtable;

ADBG(	printf("newObject %x class %s\n", obj,
		(class ? class->name->data : "<none>"));
		fflush(stdout);						)

        return (obj);
}

/*
 * Allocate a new class object.
 */
Hjava_lang_Class*
newClass(void)
{
	Hjava_lang_Class* cls;

	cls = gc_malloc(sizeof(Hjava_lang_Class), GC_ALLOC_CLASSOBJECT);

	/* We don't GC classes at the moment so secure this one */
	gc_add_ref(cls);

        /* Fill in object information */
	cls->dtable = ClassClass->dtable;

ADBG(	printf("newClass %x\n", cls);					)

        return (cls);
}

/*
 * Allocate a new array, of whatever types.
 */
Hjava_lang_Object*
newArray(Hjava_lang_Class* elclass, int count)
{
	Hjava_lang_Class* class;
	Hjava_lang_Object* obj;

	if (CLASS_IS_PRIMITIVE(elclass)) {
		obj = gc_malloc((TYPE_SIZE(elclass) * count) + sizeof(Array), GC_ALLOC_PRIMARRAY);
	}
	else {
		obj = gc_malloc((PTR_TYPE_SIZE * count) + sizeof(Array), GC_ALLOC_REFARRAY);
	}
	class = lookupArray(elclass);
	obj->dtable = class->dtable;
	ARRAY_SIZE(obj) = count;
	return (obj);
}

/*
 * Allocate a new multi-dimensional array.
 */
Hjava_lang_Object*
newMultiArray(Hjava_lang_Class* clazz, int* dims)
{
	Hjava_lang_Object* obj;
	Hjava_lang_Object** array;
	int i;
	
	obj = newArray(CLASS_ELEMENT_TYPE(clazz), dims[0]);
	if (dims[1] > 0) {
		array = OBJARRAY_DATA(obj);
		for (i = 0; i < dims[0]; i++) {
			array[i] = newMultiArray(CLASS_ELEMENT_TYPE(clazz), &dims[1]);
		}
	}

	return (obj);
}
