/*
 * gcFuncs.c
 * Methods to implement gc-related activities of objects and classes
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
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
#include "gcRefs.h"

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
	int idx;
	Hjava_lang_Class* clazz = c;
	constants* pool;

DBG(CLASSGC,
        dprintf("destroying class %s @ %p\n", clazz->name->data, c);
   )
	if (Kaffe_JavaVMArgs[0].enableVerboseGC > 0) {
		fprintf(stderr, "<GC: unloading class `%s'>\n", 
			CLASS_CNAME(clazz));
	}

	assert(!CLASS_IS_PRIMITIVE(clazz));

	/* Make sure that we don't unload fully loaded classes without
	 * classloaders.  
	 * NB: otherwise, this function must destroy any partially
	 * initialized class.  Class processing may fail at any
	 * state, and the discarded class objects destroyed.
	 */
	assert(clazz->state != CSTATE_COMPLETE || clazz->loader != 0);

        /* destroy all fields */
        if (CLASS_FIELDS(clazz) != 0) {
                Field *f = CLASS_FIELDS(clazz);
                for (i = 0; i < CLASS_NFIELDS(clazz); i++) {
                        utf8ConstRelease(f->name);
                        /* if the field was never resolved, we must release the
                         * Utf8Const to which its type field points */
                        if (!FIELD_RESOLVED(f)) {
                                utf8ConstRelease((Utf8Const*)FIELD_TYPE(f));
                        }
			f++;
                }
                KFREE(CLASS_FIELDS(clazz));
        }

        /* destroy all methods, only if this class has indeed a method table */
        if (!CLASS_IS_ARRAY(clazz) && CLASS_METHODS(clazz) != 0) {
                Method *m = CLASS_METHODS(clazz);
                for (i = 0; i < CLASS_NMETHODS(clazz); i++) {
                        utf8ConstRelease(m->name);
                        utf8ConstRelease(m->signature);
                        KFREE(m->lines);
                        KFREE(m->declared_exceptions);
                        KFREE(m->exception_table);
                        KFREE(m->c.bcode.code);	 /* aka c.ncode.ncode_start */

			/* Free ncode if necessary: this only concerns
			 * trampolines for interface <clinit> methods
			 */
			if (GC_getObjectIndex(collector, m->ncode) != -1) {
				KFREE(m->ncode);
			}
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
		}
	}
	/* free constant pool */
	if (pool->data != 0) {
		KFREE(pool->data);
	}

        /* free various other fixed things */
        KFREE(CLASS_STATICDATA(clazz));
        KFREE(clazz->dtable);
        KFREE(clazz->if2itable);
        KFREE(clazz->itable2dtable);
        KFREE(clazz->gc_layout);
	KFREE(clazz->sourcefile);

        /* The interface table for array classes points to static memory */
        if (!CLASS_IS_ARRAY(clazz)) {
                KFREE(clazz->interfaces);
        }
        utf8ConstRelease(clazz->name);
}

/*      
 * Walk the methods of a class.
 */     
static  
void    
walkMethods(Collector* collector, Method* m, int nm)
{               
        while (nm-- > 0) {
#if defined(TRANSLATOR)
                /* walk the block of jitted code conservatively.
                 * Is this really needed?
 		 */
                if (METHOD_TRANSLATED(m) && (m->accflags & ACC_NATIVE) == 0) {
                        void *mem = m->c.ncode.ncode_start;
                        if (mem != 0) {
				GC_walkConservative(collector, mem,
					GC_getObjectSize(collector, mem));
                        }
                }
#endif
                GC_markObject(collector, m->class);

                /* walk exception table in order to keep resolved catch types
                   alive */
                if (m->exception_table != 0) {
                        jexceptionEntry* eptr = &m->exception_table->entry[0];
                        int i;

                        for (i = 0; i < m->exception_table->length; i++) {
                                Hjava_lang_Class* c = eptr[i].catch_type;
                                if (c != 0 && c != UNRESOLVABLE_CATCHTYPE) {
                                        GC_markObject(collector, c);
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
walkClass(Collector* collector, void* base, uint32 size)
{
        Hjava_lang_Class* class;
        Field* fld;
        int n;
        constants* pool;
        int idx;

        class = (Hjava_lang_Class*)base;

        if (class->state >= CSTATE_PREPARED) {
                GC_markObject(collector, class->superclass);
        }

        /* walk constant pool - only resolved classes and strings count */
        pool = CLASS_CONSTANTS(class);
        for (idx = 0; idx < pool->size; idx++) {
                switch (pool->tags[idx]) {
                case CONSTANT_ResolvedClass:
			assert(!CLASS_IS_PRIMITIVE(CLASS_CLASS(idx, pool)));
                        GC_markObject(collector, CLASS_CLASS(idx, pool));
                        break;
                case CONSTANT_ResolvedString:
                        GC_markObject(collector, (void*)pool->data[idx]);
                        break;
                }
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
                                GC_markObject(collector, fld->type);
                        } /* else it's an Utf8Const that is not subject to gc */
                        fld++;
                }

                /* walk static fields that contain references */
                fld = CLASS_SFIELDS(class);
                for (n = 0; n < CLASS_NSFIELDS(class); n++) {
                        if (FIELD_RESOLVED(fld) && FIELD_ISREF(fld)) {
				/* 
				 * NB: This would break if static fields could
				 * refer to non gc-allocated objects.
				 * Check when doing GCJ.
				 */
                                GC_markObject(collector, *(void**)FIELD_ADDRESS(fld));
                        }
                        fld++;
                }
        }
 
        /* The interface table for array classes points to static memory,
         * so we must not mark it.  */
        if (!CLASS_IS_ARRAY(class)) {
                /* mark interfaces referenced by this class */
                for (n = 0; n < class->total_interface_len; n++) {
                        GC_markObject(collector, class->interfaces[n]);
                }
        } else {
                /* array classes should keep their element type alive */
		Hjava_lang_Class *etype = CLASS_ELEMENT_TYPE(class);
		if (!CLASS_IS_PRIMITIVE(etype)) {
			GC_markObject(collector, etype);
		}
        }

        /* CLASS_METHODS only points to the method array for non-array and
         * non-primitive classes */
        if (!CLASS_IS_PRIMITIVE(class) && !CLASS_IS_ARRAY(class) && CLASS_METHODS(class) != 0) {
                walkMethods(collector, CLASS_METHODS(class), CLASS_NMETHODS(class));
        }       
        GC_markObject(collector, class->loader); 
}

/*****************************************************************************
 * various walk functions functions 
 */
/*
 * Walk an array object objects.
 */             
static
void    
walkRefArray(Collector* collector, void* base, uint32 size)
{
        Hjava_lang_Object* arr; 
        int i;
        Hjava_lang_Object** ptr;
 
        arr = (Hjava_lang_Object*)base;
        if (arr->dtable == 0) {                 /* see walkObject */
                return;
        }

        ptr = OBJARRAY_DATA(arr);
        /* mark class only if not a system class (which would be anchored
         * anyway.)  */
        if (arr->dtable->class->loader != 0) {
                GC_markObject(collector, arr->dtable->class);
        }

        for (i = ARRAY_SIZE(arr); --i>= 0; ) {
                Hjava_lang_Object* el = *ptr++;
		/* 
		 * NB: This would break if some objects (i.e. class objects)
		 * are not gc-allocated.
		 */
                GC_markObject(collector, el);
        }
}

/*      
 * Walk the thread's internal context.
 * This is invoked by the garbage collector thread, which is not
 * stopped.
 *      
 * We will iterate through all threads, including the garbage collector
 * and those threads that haven't been started yet.
 */     
static  
void    
TwalkThread(Collector* collector, Hjava_lang_Thread* tid)
{       
        void *from;
        unsigned len;  
        jthread_t jtid = (jthread_t)unhand(tid)->PrivateInfo;
        
        /* Don't walk the gc thread's stack.  It was not stopped and
         * we hence don't have valid sp information.  In addition, there's
         * absolutely no reason why we should walk it at all.
         */
        if (jtid == 0 || tid == jthread_getcookie((void*)jthread_current())) {
DBG(JTHREAD,
                dprintf("%d NOT walking jtid %p\n", jthread_current(), jtid);
    )   
                return;
        }
        
        /* Ask threading system what the interesting stack range is;
         * If the thread is too young, the threading system will return
         * 0 from extract_stack.  In that case, we don't have walk anything.
         */
        if (jthread_extract_stack(jtid, &from, &len)) {
DBG(JTHREAD|DBG_GCWALK,
                dprintf("walking stack of `%s' thread\n", nameThread(tid));
    )
                /* and walk it if needed */
                GC_walkConservative(collector, from, len);
        }
}

/*
 * Walk an object.      
 */
static                  
void
walkObject(Collector* collector, void* base, uint32 size) 
{
        Hjava_lang_Object *obj = (Hjava_lang_Object*)base;
        Hjava_lang_Class *clazz;
        int *layout;
        int8* mem;
        int i, l, nbits;

        /*
         * Note that there is a window after the object is allocated but
         * before dtable is set.  In this case, we don't have to walk anything.
         */
        if (obj->dtable == 0)
                return;

        /* retrieve the layout of this object from its class */
        clazz = obj->dtable->class;

        /* class without a loader, i.e., system classes are anchored so don't
         * bother marking them.
         */
        if (clazz->loader != 0) {
                GC_markObject(collector, clazz);
        }

        layout = clazz->gc_layout;
        nbits = CLASS_FSIZE(clazz)/ALIGNMENTOF_VOIDP;

DBG(GCPRECISE,
        dprintf("walkObject `%s' ", CLASS_CNAME(clazz));
        BITMAP_DUMP(layout, nbits)
        dprintf(" (nbits=%d) %x-%x\n", nbits, base, base+size);
    )   

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
                                GC_markObject(collector, *(void **)mem);
                        }
                        i++;
                        l <<= 1;
                        mem += ALIGNMENTOF_VOIDP;
                }
                nbits -= BITMAP_BPI;
        }

        /* Special magic to handle thread objects */
        if (soft_instanceof(ThreadClass, obj)) {
                TwalkThread(collector, (Hjava_lang_Thread*)base);
        }
}

static
void
/* ARGSUSED */
finalizeObject(Collector* collector, void* ob)
{ 
        Hjava_lang_Object* obj = (Hjava_lang_Object*)ob;   
        Hjava_lang_Class* objclass = OBJECT_CLASS(obj);    
        Method* final = objclass->finalizer;               
  
        assert(final != 0);
        callMethodA(final, METHOD_INDIRECTMETHOD(final), obj, 0, 0);
} 

/*
 * Print a description of an object at a given address.
 * Single-threaded.
 */
char*
describeObject(const void* mem)
{
	static char buf[256];		/* BIG XXX */
	Hjava_lang_Class* clazz;
	Hjava_lang_String* str;
	Hjava_lang_Object* obj;
	char* c;
	jchar* jc;
	int l;

	int idx = GC_getObjectIndex(main_collector, mem);
	switch (idx) {
	case GC_ALLOC_JAVASTRING:

		str = (Hjava_lang_String*)mem;
		strcpy(buf, "java.lang.String `");
		c = buf + strlen(buf);
		jc = unhand(str)->value ? STRING_DATA(str) : 0;
		l = STRING_SIZE(str);
		while (jc && l-- > 0 && c < buf + sizeof(buf) - 2) {
			*c++ = (char)*jc++;
		}
		*c++ = '\'';
		*c = 0;
		break;

	case GC_ALLOC_CLASSOBJECT:
		clazz = (Hjava_lang_Class*)mem;
		sprintf(buf, "java.lang.Class `%s'", clazz->name ? 
			CLASS_CNAME(clazz) : "name unknown");
		break;

	case GC_ALLOC_NORMALOBJECT:
	case GC_ALLOC_FINALIZEOBJECT:
	case GC_ALLOC_REFARRAY:
	case GC_ALLOC_PRIMARRAY:
		obj = (Hjava_lang_Object*)mem;
		if (obj->dtable != 0) {
			clazz = obj->dtable->class;
			sprintf(buf, "%s", CLASS_CNAME(clazz));
		} else {
			sprintf(buf, "newly born %s",
				GC_getObjectDescription(main_collector, mem));
		}
		break;
		
	/* add more? */
		
	default:
		return ((char*)GC_getObjectDescription(main_collector, mem));
	}
	return (buf);
}

Collector*
initCollector(void)
{
	Collector *gc = createGC(gc_walk_refs);

	GC_registerGcTypeByIndex(gc, GC_ALLOC_JAVASTRING,
	    stringWalk, GC_OBJECT_NORMAL, stringDestroy, "j.l.String");
	GC_registerGcTypeByIndex(gc, GC_ALLOC_NOWALK,
	    0, GC_OBJECT_NORMAL, 0, "other-nowalk");
	GC_registerGcTypeByIndex(gc, GC_ALLOC_NORMALOBJECT,
	    walkObject, GC_OBJECT_NORMAL, 0, "obj-no-final"); 
	GC_registerGcTypeByIndex(gc, GC_ALLOC_PRIMARRAY,
	    0, GC_OBJECT_NORMAL, 0, "prim-arrays");
	GC_registerGcTypeByIndex(gc, GC_ALLOC_REFARRAY,
	    walkRefArray, GC_OBJECT_NORMAL, 0, "ref-arrays");
	GC_registerGcTypeByIndex(gc, GC_ALLOC_CLASSOBJECT,
	    walkClass, GC_OBJECT_NORMAL, destroyClass, "j.l.Class");
	GC_registerGcTypeByIndex(gc, GC_ALLOC_FINALIZEOBJECT,
	    walkObject, finalizeObject, 0, "obj-final");

	GC_registerFixedTypeByIndex(gc, GC_ALLOC_BYTECODE, "java-bytecode");
	GC_registerFixedTypeByIndex(gc, GC_ALLOC_EXCEPTIONTABLE, "exc-table");
	GC_registerFixedTypeByIndex(gc, GC_ALLOC_JITCODE, "jitcode");
	GC_registerFixedTypeByIndex(gc, GC_ALLOC_STATICDATA, "static-data");
	GC_registerFixedTypeByIndex(gc, GC_ALLOC_CONSTANT, "constants");
	GC_registerFixedTypeByIndex(gc, GC_ALLOC_FIXED, "other-fixed");
	GC_registerFixedTypeByIndex(gc, GC_ALLOC_DISPATCHTABLE, "dtable");
	GC_registerFixedTypeByIndex(gc, GC_ALLOC_METHOD, "methods");
	GC_registerFixedTypeByIndex(gc, GC_ALLOC_FIELD, "fields");
	GC_registerFixedTypeByIndex(gc, GC_ALLOC_UTF8CONST, "utf8consts");
	GC_registerFixedTypeByIndex(gc, GC_ALLOC_INTERFACE, "interfaces");
	GC_registerFixedTypeByIndex(gc, GC_ALLOC_LOCK, "locks");
	GC_registerFixedTypeByIndex(gc, GC_ALLOC_THREADCTX, "thread-ctxts");
	GC_registerFixedTypeByIndex(gc, GC_ALLOC_REF, "gc-refs");
	return (gc);
}
