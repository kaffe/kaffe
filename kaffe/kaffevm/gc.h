/*
 * gc.h
 * The garbage collector.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __gc_h
#define __gc_h

#include "gtypes.h"

/* 
 * We do not support incremental collection as this time.
 */
#undef	GC_INCREMENTAL

typedef struct _Collector Collector;
typedef void (*walk_func_t)(struct _Collector*, void*, uint32);
typedef void (*final_func_t)(struct _Collector*, void*);
typedef void (*destroy_func_t)(struct _Collector*, void*);

#define	GC_OBJECT_NORMAL	((final_func_t)0)
#define	GC_OBJECT_FIXED		((final_func_t)1)

/* Use the incremental garbage collector */
#ifndef KAFFEH
#include "mem/gc-incremental.h"
#endif

/*
 * Garbage collector interface.
 */
typedef enum {
	/* allocation types for different kinds of java objects */
	GC_ALLOC_JAVASTRING,
	GC_ALLOC_NOWALK,
	GC_ALLOC_NORMALOBJECT,
	GC_ALLOC_PRIMARRAY,
	GC_ALLOC_REFARRAY,
	GC_ALLOC_FINALIZEOBJECT,
	GC_ALLOC_JAVALOADER,

	/* allocation types related to the translator engines */
	GC_ALLOC_JITCODE,
	GC_ALLOC_JITTEMP,
	GC_ALLOC_JIT_SEQ,
	GC_ALLOC_JIT_CONST,
	GC_ALLOC_JIT_ARGS,
	GC_ALLOC_JIT_FAKE_CALL,
	GC_ALLOC_JIT_SLOTS,
	GC_ALLOC_JIT_CODEBLOCK,
	GC_ALLOC_JIT_LABELS,
	GC_ALLOC_TRAMPOLINE,
	
	/* allocation types used for java.lang.Class and its parts */
	GC_ALLOC_CLASSOBJECT,
	GC_ALLOC_BYTECODE,
	GC_ALLOC_EXCEPTIONTABLE,
	GC_ALLOC_STATICDATA,
	GC_ALLOC_CONSTANT,
	GC_ALLOC_DISPATCHTABLE,
	GC_ALLOC_METHOD,
	GC_ALLOC_FIELD,
	GC_ALLOC_INTERFACE,
	GC_ALLOC_LINENRTABLE,
	GC_ALLOC_LOCALVARTABLE,
	GC_ALLOC_DECLAREDEXC,
	GC_ALLOC_CLASSMISC,

	/* miscelanious allocation types */
	GC_ALLOC_FIXED,
	GC_ALLOC_UTF8CONST,
	GC_ALLOC_LOCK,
	GC_ALLOC_THREADCTX,
	GC_ALLOC_REF,
	GC_ALLOC_JAR,
	GC_ALLOC_CODEANALYSE,
	GC_ALLOC_CLASSPOOL,
	GC_ALLOC_VERIFIER,
	GC_ALLOC_NATIVELIB,
	GC_ALLOC_MAX_INDEX
} gc_alloc_type_t;

/*
 * Define a COM-like GC interface.
 */
struct GarbageCollectorInterface_Ops;

struct _Collector {
	struct GarbageCollectorInterface_Ops *ops;
};

struct GarbageCollectorInterface_Ops {

	void*   reserved1;
	void*   reserved2;
	void*   reserved3;
	void*	(*malloc)(Collector *, size_t size, gc_alloc_type_t type);
	void*	(*realloc)(Collector *, void* addr, size_t size, gc_alloc_type_t type);
	void	(*free)(Collector *, void* addr);

	void	(*invoke)(Collector *, int mustgc);
	void	(*invokeFinalizer)(Collector *);
	void	(*init)(Collector *);
	void	(*enable)(Collector *);

	void	(*markAddress)(Collector *, const void* addr);
	void	(*markObject)(Collector *, const void* obj);
	uint32	(*getObjectSize)(Collector *, const void* obj);
	const char* (*getObjectDescription)(Collector *, const void* obj);
	int	(*getObjectIndex)(Collector *, const void* obj);
	void*	(*getObjectBase)(Collector *, const void* obj);

	void	(*walkMemory)(Collector *, void *addr);
	void	(*walkConservative)(Collector *, 
			const void* addr, uint32 length);

	void	(*registerFixedTypeByIndex)(Collector *, 
			int gc_index, const char *description);

	void 	(*registerGcTypeByIndex)(Collector *, 
			int gc_index,
			walk_func_t walk, final_func_t final, 
			destroy_func_t destroy, const char
					 *description);
	struct Hjava_lang_Throwable *(*throwOOM)(Collector *);

	void 	(*enableGC)(Collector *);
	void 	(*disableGC)(Collector *);
};

Collector* createGC(void (*_walkRootSet)(Collector*));

/*
 * Convenience macros
 */
#define GC_malloc(G, size, type)	\
    ((G)->ops->malloc)((Collector*)(G), (size), (type))
#define GC_realloc(G, addr, size, type)	\
    ((G)->ops->realloc)((Collector*)(G), (addr), (size), (type))
#define GC_free(G, addr)		\
    ((G)->ops->free)((Collector*)(G), (addr))
#define GC_invoke(G, mustgc)		\
    ((G)->ops->invoke)((Collector*)(G), (mustgc))
#define GC_invokeFinalizer(G)		\
    ((G)->ops->invokeFinalizer)((Collector*)(G))
#define GC_init(G)		\
    ((G)->ops->init)((Collector*)(G))
#define GC_enable(G)		\
    ((G)->ops->enable)((Collector*)(G))
#define GC_throwOOM(G)		\
    ((G)->ops->throwOOM)((Collector*)(G))
#define GC_markAddress(G, addr)		\
    ((G)->ops->markAddress)((Collector*)(G), (addr))

#if !defined(KAFFEH)
static inline void GC_markObject(void *g, void *addr)
{
	if (addr)
		((Collector*) g)->ops->markObject((Collector*) g, addr);
}
#endif

#define GC_getObjectSize(G, obj)	\
    ((G)->ops->getObjectSize)((Collector*)(G), (obj))
#define GC_getObjectDescription(G, obj)	\
    ((G)->ops->getObjectDescription)((Collector*)(G), (obj))
#define GC_getObjectIndex(G, obj)	\
    ((G)->ops->getObjectIndex)((Collector*)(G), (obj))
#define GC_getObjectBase(G, obj)	\
    ((G)->ops->getObjectBase)((Collector*)(G), (obj))
#define GC_walkMemory(G, addr)	\
    ((G)->ops->walkMemory)((Collector*)(G), (addr))
#define GC_walkConservative(G, addr, len)		\
    ((G)->ops->walkConservative)((Collector*)(G), (addr), (len))
#define GC_registerFixedTypeByIndex(G, idx, desc)	\
    ((G)->ops->registerFixedTypeByIndex)((Collector*)(G),   \
				(idx), (desc))
#define GC_registerGcTypeByIndex(G, idx, walk, final, destroy, desc)	\
    ((G)->ops->registerGcTypeByIndex)((Collector*)(G), 	     \
				(idx), (walk), (final), (destroy), (desc))
#define GC_enableGC(G)		\
    ((G)->ops->enableGC)((Collector*)(G));
#define GC_disableGC(G)		\
    ((G)->ops->disableGC)((Collector*)(G));

/*
 * Compatibility macros to access GC functions
 */
extern Collector* main_collector;

#define	gc_malloc(A,B)	    GC_malloc(main_collector,A,B)
#define	gc_calloc(A,B,C)    GC_malloc(main_collector,(A)*(B),C)
#define	gc_realloc(A,B,C)   GC_realloc(main_collector,(A),(B),C)
#define	gc_free(A)	    GC_free(main_collector,(A))

#define	invokeGC()	    GC_invoke(main_collector,1)
#define	adviseGC()	    GC_invoke(main_collector,0)
#define	invokeFinalizer()   GC_invokeFinalizer(main_collector)

#define gc_throwOOM()	    GC_throwOOM(main_collector)

#define gc_enableGC()	    GC_enableGC(main_collector)
#define gc_disableGC()	    GC_disableGC(main_collector)

#include "gcRefs.h"
extern char* describeObject(const void* mem);
#endif
