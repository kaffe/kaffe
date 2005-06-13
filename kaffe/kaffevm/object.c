/*
 * object.c
 * Handle create and subsequent garbage collection of objects.
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

#define	ADBG(s)

#include "config.h"
#include "debug.h"
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
#include "thread.h"
#include "jvmpi_kaffe.h"

Hjava_lang_Object*
newObjectChecked(Hjava_lang_Class* class, errorInfo *info)
{
	Hjava_lang_Object* obj;

	if (CLASS_IS_INTERFACE(class)) {
		postExceptionMessage(info,
				     JAVA_LANG(InstantiationError),
				     "(class: %s) "
				     "Abstract class.",
				     CLASS_CNAME(class));
		return (NULL);
	}
	obj = gc_malloc((size_t)(CLASS_FSIZE(class)), class->alloc_type);

	if (!obj) {
		postOutOfMemory(info);
	} else {
	    /* Fill in object information */
	    KaffeVM_setFinalizer(obj, KGC_DEFAULT_FINALIZER);
	    obj->vtable = class->vtable;

#if defined(ENABLE_JVMPI)
	    if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_OBJECT_ALLOC) )
	    {
		    JVMPI_Event ev;
		    
		    jvmpiFillObjectAlloc(&ev, obj);
		    jvmpiPostEvent(&ev);
	    }
#endif
	    
	}
DBG(NEWOBJECT,
	dprintf("newObject %p class %s\n", obj,
		(class ? class->name->data : "<none>"));
    );
        return (obj);
}
/*
 * Create a new Java object.
 *
 * Throws an out of memory exception if the object cannot be allocated.
 * NB: newObject must be called in user mode.
 */
Hjava_lang_Object*
newObject(Hjava_lang_Class* class)
{
	errorInfo info;
	Hjava_lang_Object *obj = newObjectChecked(class, &info);

	if (obj == 0) {
		throwError(&info);
	}

        return (obj);
}

/*
 * Allocate a new class object.
 * We make Class objects roots if class gc is disabled.
 *
 * Returns null if the class object cannot be allocated because
 * of out of memory.
 */
Hjava_lang_Class*
newClass(void)
{
	Hjava_lang_Class* cls;

	cls = gc_malloc(sizeof(Hjava_lang_Class), KGC_ALLOC_CLASSOBJECT);
	if (cls == NULL) {
		goto done;
	}

	/* Turn off class gc */
	if (Kaffe_JavaVMArgs.enableClassGC == 0) {
		if (!gc_add_ref(cls)) {
			return NULL;
		}
	}

        /* Fill in object information */
	KaffeVM_setFinalizer(cls, KGC_DEFAULT_FINALIZER);
	cls->head.vtable = getClassVtable();
done:
DBG(NEWOBJECT,
	dprintf("newClass @%p\n", cls);
    );

        return (cls);
}

/*
 * Allocate a new array, of whatever types.
 */
Hjava_lang_Object*
newArrayChecked(Hjava_lang_Class* elclass, jsize count, errorInfo *info)
{
	Hjava_lang_Class* class = NULL;
	Hjava_lang_Object* obj = NULL;

	if ((class = lookupArray(elclass, info)) != NULL) {
		size_t total_count;
		
		if (CLASS_IS_PRIMITIVE(elclass) || elclass == PtrClass) {
			total_count = (TYPE_SIZE(elclass) * count) +
				ARRAY_DATA_OFFSET;
			obj = gc_malloc(total_count,
					KGC_ALLOC_PRIMARRAY);
		}
		else {
			total_count = (PTR_TYPE_SIZE * count) +
				ARRAY_DATA_OFFSET;

			obj = gc_malloc(total_count,
					KGC_ALLOC_REFARRAY);
		}
		if (obj != NULL) {
		        KaffeVM_setFinalizer(obj, KGC_DEFAULT_FINALIZER);
			obj->vtable = class->vtable;
			ARRAY_SIZE(obj) = count;

#if defined(ENABLE_JVMPI)
			if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_OBJECT_ALLOC) )
			{
				JVMPI_Event ev;

				jvmpiFillObjectAlloc(&ev, obj);
				jvmpiPostEvent(&ev);
			}
#endif
			
		} else {
			postOutOfMemory(info);
		}
	}
DBG(NEWOBJECT,
	dprintf("newArray %p class %s count %d\n", obj,
		(class ? class->name->data : "<none>"), count);
    );
	return (obj);
}

/*
 * Allocate a new array, of whatever types.
 */
Hjava_lang_Object*
newArray(Hjava_lang_Class* elclass, jsize count)
{
	Hjava_lang_Object* obj;
	errorInfo info;

	obj = newArrayChecked(elclass, count, &info);
	if (!obj) {
		throwError(&info);
	}
	return (obj);
}

/*
 * Allocate a new multi-dimensional array.
 */
Hjava_lang_Object*
newMultiArrayChecked(Hjava_lang_Class* clazz, int* dims, errorInfo *einfo)
{
  Hjava_lang_Object* obj;
  Hjava_lang_Object** stack;
  Hjava_lang_Class* prevclazz;
  Hjava_lang_Object** array;
  int i, j, k, ndims, fullsize;
  int stacksize, stackptr;

  for (i=0,fullsize=1;dims[i+1] >= 0; i++)
    fullsize += fullsize*dims[i];
  ndims = i+1;

  stack = (Hjava_lang_Object **)
    KMALLOC(fullsize * sizeof(Hjava_lang_Object *));
  if (stack == NULL)
    {
      postOutOfMemory(einfo);
      return NULL;
    }
  
  obj = newArrayChecked(CLASS_ELEMENT_TYPE(clazz), (jsize)dims[0], einfo);
  if (!obj)
    return NULL;
  
  stack[0] = obj;
  stackptr = 0;
  stacksize = 1;
  for (j=0;j<ndims-1;j++)
    {
      int localdim = dims[j];
      
      prevclazz = CLASS_ELEMENT_TYPE(clazz);
      for (i=0;i<stacksize;i++)
	{
	  int localptr = stackptr + stacksize + i*localdim;

	  array = OBJARRAY_DATA(stack[stackptr+i]);
	  for (k=0;k<localdim;k++)
	  {
	    stack[localptr+k] = array[k] = 
	      newArrayChecked(CLASS_ELEMENT_TYPE(prevclazz), (jsize)dims[j+1], einfo);
	    if (array[k] == NULL)
	      return NULL;
	  }
	}
      clazz = prevclazz;
      stackptr += stacksize;
      stacksize *= dims[j];
    }

  KFREE(stack);

  return obj;
}

/*
 * Allocate a new multi-dimensional array.
 */
Hjava_lang_Object*
newMultiArray(Hjava_lang_Class* clazz, int* dims)
{
	Hjava_lang_Object* obj;
	errorInfo einfo;

	obj = newMultiArrayChecked(clazz, dims, &einfo);
	if (!obj) {
		throwError(&einfo);
	}
	return (obj);
}

