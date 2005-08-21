/*
 * gcFuncs.c
 * Methods to implement gc-related activities of objects and classes
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *	Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *   
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
/*
 * This file contains those functions that have to do with gc
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "defs.h"
#include "gtypes.h"
#include "slots.h"
#include "access.h"
#include "object.h"
#include "errors.h"
#include "code.h"
#include "file.h"
#include "readClass.h"
#include "classMethod.h"
#include "baseClasses.h"
#include "stringSupport.h"
#include "thread.h"
#include "jthread.h"
#include "itypes.h"
#include "bytecode.h"
#include "exception.h"
#include "md.h"
#include "external.h"
#include "lookup.h"
#include "support.h"
#include "gc.h"
#include "locks.h"
#include "md.h"
#include "jni.h"
#include "soft.h"
#include "thread.h"
#include "methodCache.h"
#include "jvmpi_kaffe.h"
#include "methodcalls.h"

/*****************************************************************************
 * Class-related functions
 */

/*
 * Destroy a class object.
 */
static void
/* ARGSUSED */
destroyClass(Collector *collector, void* c)
{
        int i;
	unsigned int idx;
	Hjava_lang_Class* clazz = c;
	constants* pool;

DBG(CLASSGC,
        dprintf("destroying class %s @ %p\n",
		clazz->name ? clazz->name->data : "newborn", c);
   );
	assert(!CLASS_IS_PRIMITIVE(clazz)); 

	/* NB: Make sure that we don't unload fully loaded classes without
	 * classloaders.  This is wrong and indicate of a bug.
	 *
	 * NB: Note that this function must destroy any partially
	 * initialized class.  Class processing may fail at any
	 * state, and the discarded class objects destroyed.
	 */
	assert(clazz->state != CSTATE_COMPLETE || clazz->loader != 0);

#if defined(ENABLE_JVMPI)
	if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_CLASS_UNLOAD) )
	{
		JVMPI_Event ev;

		ev.event_type = JVMPI_EVENT_CLASS_UNLOAD;
		ev.u.class_unload.class_id = c;
		jvmpiPostEvent(&ev);
	}
#endif

	if (Kaffe_JavaVMArgs.enableVerboseGC > 0 && clazz->name) {
		DBG(CLASSGC,
			dprintf("<GC: unloading class `%s'>\n",
				CLASS_CNAME(clazz));
		);
	}

        /* destroy all fields */
        if (CLASS_FIELDS(clazz) != 0) {
                Field *f = CLASS_FIELDS(clazz);
                for (i = 0; i < CLASS_NFIELDS(clazz); i++) {
                        utf8ConstRelease(f->name);
                        /* if the field was never resolved, we must release the
                         * Utf8Const to which its type field points */
			utf8ConstRelease(f->signature);
			f++;
                }
                KFREE(CLASS_FIELDS(clazz));
        }

        /* destroy all methods, only if this class has indeed a method table */
        if (!CLASS_IS_ARRAY(clazz) && CLASS_METHODS(clazz) != 0) {
                Method *m = CLASS_METHODS(clazz);
                for (i = 0; i < CLASS_NMETHODS(clazz); i++) {
			void *ncode = NULL;

			if (!CLASS_IS_INTERFACE(clazz))
			{
				ncode = METHOD_NATIVECODE(m);
				if (METHOD_JITTED(m)) {
#if defined(TRANSLATOR) && defined (MD_UNREGISTER_JIT_EXCEPTION_INFO)
					MD_UNREGISTER_JIT_EXCEPTION_INFO (m->c.ncode.ncode_start,
									  ncode,
									  m->c.ncode.ncode_end);
#endif
#if defined(ENABLE_JVMPI)
					if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_COMPILED_METHOD_UNLOAD)  )
					{
						JVMPI_Event ev;

						ev.event_type = JVMPI_EVENT_COMPILED_METHOD_UNLOAD;
						ev.u.compiled_method_unload.
							method_id = m;
						jvmpiPostEvent(&ev);
					}
#endif
				}
			}
			utf8ConstRelease(m->name);
                        utf8ConstRelease(METHOD_SIG(m));
                        KFREE(METHOD_PSIG(m));
                        KFREE(m->lines);
			KFREE(m->lvars);
			if( m->ndeclared_exceptions != -1 )
			  KFREE(m->declared_exceptions);
                        KFREE(m->exception_table);

			/* ncode is swept by the GC. */
			m++;
                }
                KFREE(CLASS_METHODS(clazz));
        }

        /* release remaining refs to utf8consts in constant pool */
	pool = CLASS_CONSTANTS (clazz);
	for (idx = 0; idx < pool->size; idx++) {
		switch (pool->tags[idx]) {
		case CONSTANT_String:	/* unresolved strings */
		case CONSTANT_Utf8:
			utf8ConstRelease(WORD2UTF(pool->data[idx]));
			break;
		default:
			break;
		}
	}
	/* free constant pool */
	if (pool->data != NULL)
	  {
	    KFREE(pool->data);
	  }

        /* free various other fixed things */
        KFREE(CLASS_STATICDATA(clazz));

       	if(clazz->vtable != NULL)
	  {
	    /* The native methods in the vtable are swept by the GC. */
	    KFREE(clazz->vtable);
	  }

        KFREE(clazz->if2itable);

	if (clazz->implementors != NULL)
	  {
	    uintp len, uidx;

	    len = (uintp)clazz->implementors[0] + 1;
	    for (uidx = 1; uidx < len; uidx++)
	      {
		void *impl = clazz->implementors[uidx];
		Hjava_lang_Class **impl_clazz;

		if (impl == NULL)
		  continue;
		
		impl_clazz = (Hjava_lang_Class **)KGC_getObjectBase(collector, impl);
		assert(impl_clazz != NULL);

		/* We must walk the list of interfaces for this class and
		 * unregister this interface. As the class should also be
		 * freed (in the other case this interface would not have 
		 * been destroyed), there is no problem with this.
		 */
		for (i = 0; i < (*impl_clazz)->total_interface_len; i++)
		  if ((*impl_clazz)->interfaces[i] == clazz)
		    {
		      (*impl_clazz)->interfaces[i] = NULL;
		      /* We cannot break here because there may exist
		       * duplicates in the current list.
		       */
		    }
	      }
	    
	    KFREE(clazz->implementors);
	  }

	if (clazz->interfaces)
		gc_rm_ref(clazz->interfaces);

	if( clazz->itable2dtable )
	{
	  for (i = 0; i < clazz->total_interface_len; i++) {
	    Hjava_lang_Class* iface = clazz->interfaces[i];
	    
	    /* only if interface has not been freed already. We
	     * update the implementors section of the interface
	     * accordingly.
	     */
	    if (iface != NULL)
	      iface->implementors[clazz->impl_index] = NULL;
	  }
	  /* The itable2dtable table will be automatically swept by the
	   * GC when the class becomes unused as it is only marked while
	   * the class is being walked (see walkClass).
	   */
	  KGC_rmRef(collector, clazz->itable2dtable);
	}
	if (clazz->gc_layout != NULL && clazz->superclass != NULL &&
	    clazz->superclass->gc_layout != clazz->gc_layout)
	  KFREE(clazz->gc_layout);

	KFREE(clazz->sourcefile);
	KFREE(clazz->inner_classes);

        utf8ConstRelease(clazz->name);
}

/*
 * Walk the methods of a class.
 */
static
void
walkMethods(Collector* collector, void *gc_info, Method* m, int nm)
{
        while (nm-- > 0) {
	        int objIndex;

                KGC_markObject(collector, gc_info, m->class);
		
		objIndex = KGC_getObjectIndex(collector, m->ncode);
		if (objIndex == KGC_ALLOC_JITCODE 
                    || objIndex == KGC_ALLOC_TRAMPOLINE)
		  KGC_markObject(collector, gc_info, m->ncode);
		
		objIndex = KGC_getObjectIndex(collector, m->c.bcode.code);
		if (objIndex == KGC_ALLOC_JITCODE 
                    || objIndex == KGC_ALLOC_TRAMPOLINE 
                    || objIndex == KGC_ALLOC_BYTECODE)
		  KGC_markObject(collector, gc_info, m->c.bcode.code);

                /* walk exception table in order to keep resolved catch types
                   alive */
                if (m->exception_table != 0) {
                        jexceptionEntry* eptr = &m->exception_table->entry[0];
                        unsigned int i;

                        for (i = 0; i < m->exception_table->length; i++) {
                                Hjava_lang_Class* c = eptr[i].catch_type;
                                if (c != 0 && c != UNRESOLVABLE_CATCHTYPE) {
                                        KGC_markObject(collector, gc_info, c);
                                }
                        }
                }
                m++;
        }
}

/*
 * Walk a class object.
 */
static void
walkClass(Collector* collector, void *gc_info, void* base, uint32 size UNUSED)
{
        Hjava_lang_Class* class;
        Field* fld;
        int n;
        constants* pool;
        unsigned int idx;
	iLock *lk;

        class = (Hjava_lang_Class*)base;

DBG(GCPRECISE,
        dprintf("walkClass `%s' state=%d\n", CLASS_CNAME(class), class->state);
    );

        lk = GET_HEAVYLOCK(class->lock); 
        if (lk != NULL
	    && KGC_getObjectIndex(collector, lk) == KGC_ALLOC_LOCK)
           KGC_markObject(collector, gc_info, lk);

	lk = GET_HEAVYLOCK(class->head.lock);
        if (lk != NULL
	    && KGC_getObjectIndex(collector, lk) == KGC_ALLOC_LOCK)
           KGC_markObject(collector, gc_info, lk);

        if (class->state >= CSTATE_PREPARED) {
	  KGC_markObject(collector, gc_info, class->superclass);
        } 

	if (class->itable2dtable != NULL)
	  {
	    unsigned int len = class->if2itable[class->total_interface_len];
	    KGC_markObject(collector, gc_info, class->itable2dtable);

	    for (idx = 1; idx < len; idx++)
	      {
		void *method = class->itable2dtable[idx];
		int objIndex;
		
		if (method == (void*)-1)
		  continue;
		
		objIndex = KGC_getObjectIndex(collector, method);
		
		if (objIndex == KGC_ALLOC_JITCODE 
                    || objIndex == KGC_ALLOC_TRAMPOLINE)
		  KGC_markObject(collector, gc_info, method);
	      }
	  }

        /* walk constant pool - only resolved classes and strings count */
        pool = CLASS_CONSTANTS(class);
        for (idx = 0; idx < pool->size; idx++) {
                switch (pool->tags[idx]) {
                case CONSTANT_ResolvedClass:
			assert(!CLASS_IS_PRIMITIVE(CLASS_CLASS(idx, pool)));
                        KGC_markObject(collector, gc_info, CLASS_CLASS(idx, pool));
                        break;
                case CONSTANT_ResolvedString:
                        KGC_markObject(collector, gc_info, (void*)pool->data[idx]);
                        break;
		default:
			break;
                }
        }

	/* walk the local vtable */
	if (class->vtable != NULL && !CLASS_IS_PRIMITIVE(class))
	  for (idx = 0; idx < class->msize; idx++)
	    {
	      void *method = class->vtable->method[idx];
	      int objIndex = KGC_getObjectIndex(collector, method);

	      if (objIndex == KGC_ALLOC_JITCODE 
                  || objIndex == KGC_ALLOC_TRAMPOLINE 
                  || objIndex == KGC_ALLOC_BYTECODE)
		KGC_markObject(collector, gc_info, method);
	    }

        /*
         * NB: We suspect that walking the class pool should suffice if
         * we ensured that all classes referenced from this would show up
         * as a ResolvedClass entry in the pool.  However, this is not
         * currently the case: for instance, resolved field type are not
         * marked as resolved in the constant pool, even though they do
         * have an index there! XXX
         *
         * The second hypothesis is that if the class is loaded by the
         * system and thus anchored, then everything that we can reach from
         * here is anchored as well.  If that property holds, we should be
         * able to just return if class->loader == null here.   XXX
         */
        /* walk fields */
        if (CLASS_FIELDS(class) != 0) {

                /* walk all fields to keep their types alive */
                fld = CLASS_FIELDS(class);
                for (n = 0; n < CLASS_NFIELDS(class); n++) {
			/* don't mark field types that are primitive classes */
                        if (FIELD_RESOLVED(fld)
				&& !CLASS_IS_PRIMITIVE(fld->type))
			{
				if (!CLASS_GCJ(fld->type)) {
					KGC_markObject(collector, gc_info, fld->type);
				}
                        } /* else it's an Utf8Const that is not subject to gc */
                        fld++;
                }

                /* walk static fields that contain references */
                fld = CLASS_SFIELDS(class);
                for (n = 0; n < CLASS_NSFIELDS(class); n++) {
		    	/* Note that we must resolve field types eagerly
			 * in processClass for gcj code cause it may never
			 * call anything like getField to get the field
			 * type resolved.  This can happen for such types as [C
			 */
                        if (FIELD_RESOLVED(fld) && FIELD_ISREF(fld)) {
				void **faddr = (void**)FIELD_ADDRESS(fld);
#if defined (HAVE_GCJ_SUPPORT)
/* 1. GCJ work-around, see
 * http://sourceware.cygnus.com/ml/java-discuss/1999-q4/msg00379.html
 */
				if (FIELD_TYPE(fld) == getStringClass()) {
					KGC_markAddress(collector, gc_info, *faddr);
				} else {
					KGC_markObject(collector, gc_info, *faddr);
				}
#else
				KGC_markObject(collector, gc_info, *faddr);
#endif
                        }
                        fld++;
                }
        }

        /* The interface table for array classes points to static memory,
         * so we must not mark it.  */
        if (!CLASS_IS_ARRAY(class)) {
                /* mark interfaces referenced by this class */
                for (n = 0; n < class->total_interface_len; n++) {
                        KGC_markObject(collector, gc_info, class->interfaces[n]);
                }
        } else {
                /* array classes should keep their element type alive */
		Hjava_lang_Class *etype = CLASS_ELEMENT_TYPE(class);
		if (etype && !CLASS_IS_PRIMITIVE(etype)) {
			KGC_markObject(collector, gc_info, etype);
		}
        }

	/* Now we walk the interface table pointer. */
	if (class->itable2dtable != NULL)
	  {
	    KGC_markObject(collector, gc_info, class->itable2dtable);
	    /* We want the number of interfaces registered in the table. As
	     * this number is not recorded in the table we recompute it
	     * quickly using if2itable. (See classMethod.c/buildInterfaceDispatchTable).
	     */
	    for (idx = 1, n = 0; n < class->total_interface_len; n++)
	      {
		void *iface = class->itable2dtable[idx];
		
		KGC_markObject(collector, gc_info, iface);
		idx += class->interfaces[n]->msize+1;
	      }
	  }

        /* CLASS_METHODS only points to the method array for non-array and
         * non-primitive classes */
        if (!CLASS_IS_PRIMITIVE(class) && !CLASS_IS_ARRAY(class) && CLASS_METHODS(class) != 0) {
                walkMethods(collector, gc_info, CLASS_METHODS(class), CLASS_NMETHODS(class));
        }
        KGC_markObject(collector, gc_info, class->loader);
	KGC_markObject(collector, gc_info, class->signers);
	KGC_markObject(collector, gc_info, class->protectionDomain);
	KGC_markObject(collector, gc_info, class->constructor);
}

/*****************************************************************************
 * various walk functions functions
 */
/*
 * Walk an array object objects.
 */
static
void
walkRefArray(Collector* collector, void *gc_info, void* base, uint32 size UNUSED)
{
        Hjava_lang_Object* arr;
        int i;
	iLock *lk;
        Hjava_lang_Object** ptr;

        arr = (Hjava_lang_Object*)base;
        if (arr->vtable == 0) {                 /* see walkObject */
                return;
        }

	lk = GET_HEAVYLOCK(arr->lock);
   	if (lk != NULL && KGC_getObjectIndex(collector, lk) == KGC_ALLOC_LOCK)
	  KGC_markObject(collector, gc_info, lk);

DBG(GCPRECISE,
        dprintf("walkRefArray `%s' (num=%d)\n", CLASS_CNAME(arr->vtable->class), ARRAY_SIZE(arr));
   );

	ptr = OBJARRAY_DATA(arr);
        /* mark class only if not a system class (which would be anchored
         * anyway.)  */
        if (arr->vtable->class->loader != 0) {
                KGC_markObject(collector, gc_info, arr->vtable->class);
        }

        for (i = ARRAY_SIZE(arr); i > 0; i--) {
                Hjava_lang_Object* el = *ptr++;
		/*
		 * NB: This would break if some objects (i.e. class objects)
		 * are not gc-allocated.
		 */
		KGC_markObject(collector, gc_info, el);
        }
}

/*
 * Walk an object.
 */
static
void
walkObject(Collector* collector, void *gc_info, void* base, uint32 size)
{
        Hjava_lang_Object *obj = (Hjava_lang_Object*)base;
        Hjava_lang_Class *clazz;
        int *layout;
        int8* mem;
        unsigned int i;
	int l, nbits;
	iLock *lk;

        /*
         * Note that there is a window after the object is allocated but
         * before dtable is set.  In this case, we don't have to walk anything.
         */
        if (obj->vtable == 0)
                return;

        /* retrieve the layout of this object from its class */
        clazz = obj->vtable->class;

        /* class without a loader, i.e., system classes are anchored so don't
         * bother marking them.
         */
        if (clazz->loader != 0) {
                KGC_markObject(collector, gc_info, clazz);
        }

	lk = GET_HEAVYLOCK(obj->lock);
	if (lk != NULL && KGC_getObjectIndex(collector, lk) == KGC_ALLOC_LOCK)
	  KGC_markObject(collector, gc_info, lk);

        layout = clazz->gc_layout;
        nbits = CLASS_FSIZE(clazz)/ALIGNMENTOF_VOIDP;

DBG(GCPRECISE,
        dprintf("walkObject `%s' ", CLASS_CNAME(clazz));
        BITMAP_DUMP(layout, nbits)
        dprintf(" (nbits=%d) %p-%p\n", nbits, base, ((char *)base) + size);
    );

        assert(CLASS_FSIZE(clazz) > 0);
        assert(size > 0);

        mem = (int8 *)base;

        /* optimize this! */
        while (nbits > 0) {
                /* get next integer from bitmap */
                l = *layout++;
                i = 0;
                while (i < BITMAP_BPI) {
                        /* skip the rest if no refs left */
                        if (l == 0) {
                                mem += (BITMAP_BPI - i) * ALIGNMENTOF_VOIDP;
                                break;
                        }

                        if (l < 0) {
                                /* we know this pointer points to gc'ed memory
                                 * there is no need to check - go ahead and
                                 * mark it.  Note that p may or may not point
                                 * to a "real" Java object.
                                 */
				void *p = *(void **)mem;
				KGC_markObject(collector, gc_info, p);
                        }
                        i++;
                        l <<= 1;
                        mem += ALIGNMENTOF_VOIDP;
                }
                nbits -= BITMAP_BPI;
        }
}

/*
 * Walk a loader object.
 */
static
void
walkLoader(Collector* collector, void *gc_info, void* base, uint32 size)
{
        walkObject(collector, gc_info, base, size);
        walkClassEntries(collector, gc_info, (Hjava_lang_ClassLoader*)base);
}

static void
/* ARGSUSED */
finalizeObject(Collector* collector UNUSED, void* ob)
{
        Hjava_lang_Object* obj = (Hjava_lang_Object*)ob;

	if (!obj->vtable) {
		/* Suppose we catch ThreadDeath inside newObject() */
		return;
	}
	assert(obj->finalizer_call != NULL);
	((KaffeVM_Finalizer)obj->finalizer_call)(obj);
}

/*
 * Print a description of an object at a given address.
 * Single-threaded.
 */
const char*
describeObject(const void* mem)
{
	static char buf[256];		/* BIG XXX */
	const Hjava_lang_Class* clazz;
	const Hjava_lang_String* str;
	const Hjava_lang_Object* obj;
	char* c;
	jchar* jc;
	int l;

	int idx = KGC_getObjectIndex(main_collector, mem);
	switch (idx) {
	case KGC_ALLOC_JAVASTRING:

		str = (const Hjava_lang_String*)mem;
		strcpy(buf, "java.lang.String `");
		c = buf + strlen(buf);
		jc = unhand(str)->value ? STRING_DATA(str) : NULL;
		l = STRING_SIZE(str);
		while (jc && l-- > 0 && c < buf + sizeof(buf) - 2) {
			*c++ = (char)*jc++;
		}
		*c++ = '\'';
		*c = 0;
		break;

	case KGC_ALLOC_CLASSOBJECT:
		clazz = (const Hjava_lang_Class*)mem;
		sprintf(buf, "java.lang.Class `%s'", clazz->name ?
			CLASS_CNAME(clazz) : "name unknown");
		break;

	case KGC_ALLOC_JAVALOADER:
	case KGC_ALLOC_NORMALOBJECT:
	case KGC_ALLOC_FINALIZEOBJECT:
	case KGC_ALLOC_REFARRAY:
	case KGC_ALLOC_PRIMARRAY:
		obj = (const Hjava_lang_Object*)mem;
		if (obj->vtable != 0) {
			clazz = obj->vtable->class;
			sprintf(buf, "%s", CLASS_CNAME(clazz));
		} else {
			sprintf(buf, "newly born %s",
				KGC_getObjectDescription(main_collector, mem));
		}
		break;

	/* add more? */

	default:
		return KGC_getObjectDescription(main_collector, mem);
	}
	return (buf);
}

Collector*
initCollector(void)
{
	Collector *gc = createGC();

	DBG(INIT, dprintf("initCollector()\n"); );

	KGC_registerGcTypeByIndex(gc, KGC_ALLOC_JAVASTRING,
	    stringWalk, finalizeObject, stringDestroy, "j.l.String");
	KGC_registerGcTypeByIndex(gc, KGC_ALLOC_NOWALK,
	    NULL, KGC_OBJECT_NORMAL, NULL, "other-nowalk");
	KGC_registerGcTypeByIndex(gc, KGC_ALLOC_NORMALOBJECT,
	    walkObject, KGC_OBJECT_NORMAL, NULL, "obj-no-final");
	KGC_registerGcTypeByIndex(gc, KGC_ALLOC_PRIMARRAY,
	    NULL, finalizeObject, NULL, "prim-arrays");
	KGC_registerGcTypeByIndex(gc, KGC_ALLOC_REFARRAY,
	    walkRefArray, finalizeObject, NULL, "ref-arrays");
	KGC_registerGcTypeByIndex(gc, KGC_ALLOC_CLASSOBJECT,
	    walkClass, finalizeObject, destroyClass, "j.l.Class");
	KGC_registerGcTypeByIndex(gc, KGC_ALLOC_FINALIZEOBJECT,
	    walkObject, finalizeObject, NULL, "obj-final");
	KGC_registerGcTypeByIndex(gc, KGC_ALLOC_JAVALOADER,
	    walkLoader, finalizeObject, destroyClassLoader,
	    "j.l.ClassLoader");
	KGC_registerGcTypeByIndex(gc, KGC_ALLOC_THREADCTX, 
	    NULL, KGC_OBJECT_NORMAL, NULL, "thread-ctxts");
	KGC_registerGcTypeByIndex(gc, KGC_ALLOC_INTERFACE,
  	    NULL, KGC_OBJECT_NORMAL, NULL, "interfaces");
	KGC_registerGcTypeByIndex(gc, KGC_ALLOC_INTERFACE_TABLE,
            NULL, KGC_OBJECT_NORMAL, NULL, "interface table");
	KGC_registerGcTypeByIndex(gc, KGC_ALLOC_TRAMPOLINE,
	    NULL, KGC_OBJECT_NORMAL, NULL, "trampoline");
	KGC_registerGcTypeByIndex(gc, KGC_ALLOC_JITCODE,
	    NULL, KGC_OBJECT_NORMAL, NULL, "jit-code");
	KGC_registerGcTypeByIndex(gc, KGC_ALLOC_BYTECODE,
	    NULL, KGC_OBJECT_NORMAL, NULL, "java-bytecode");
	KGC_registerGcTypeByIndex(gc, KGC_ALLOC_LOCK,
	    NULL, KGC_OBJECT_NORMAL, KaffeLock_destroyLock, "locks");

	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_STATIC_THREADDATA, "thread-data");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_EXCEPTIONTABLE, "exc-table");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_STATICDATA, "static-data");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_CONSTANT, "constants");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_FIXED, "other-fixed");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_DISPATCHTABLE, "dtable");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_METHOD, "methods");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_FIELD, "fields");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_UTF8CONST, "utf8consts");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_REF, "gc-refs");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_JITTEMP, "jit-temp-data");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_JAR, "jar");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_CODEANALYSE, "code-analyse");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_CLASSPOOL, "class-pool");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_LINENRTABLE, "linenr-table");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_LOCALVARTABLE, "lvar-table");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_DECLAREDEXC, "declared-exc");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_CLASSMISC, "class-misc");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_VERIFIER, "verifier");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_NATIVELIB, "native-lib");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_JIT_SEQ, "jit-seq");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_JIT_CONST, "jit-const");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_JIT_ARGS, "jit-args");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_JIT_FAKE_CALL, "jit-fake-call");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_JIT_SLOTS, "jit-slots");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_JIT_CODEBLOCK, "jit-codeblock");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_JIT_LABELS, "jit-labels");
	KGC_registerFixedTypeByIndex(gc, KGC_ALLOC_VMWEAKREF, "vm-weak-ref");

	DBG(INIT, dprintf("initCollector() done\n"); );
	return (gc);
}
