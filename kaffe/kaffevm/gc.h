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

/* Enable incremental collection - recommended but if you have odd behaviour
 * turn this option off - non-incremental collection is more debugged.
 */
#undef	GC_INCREMENTAL

/* Use the incremental garbage collector */
#include "mem/gc-incremental.h"

/*
 * Garbage collector interface.
 */
#define	GC_ALLOC_NORMAL		0
#define	GC_ALLOC_NOWALK		1
#define	GC_ALLOC_FIXED		2
#define	GC_ALLOC_NORMALOBJECT	3
#define	GC_ALLOC_PRIMARRAY	4
#define	GC_ALLOC_REFARRAY	5
#define	GC_ALLOC_CLASSOBJECT	6
#define	GC_ALLOC_FINALIZEOBJECT	7
#define	GC_ALLOC_METHOD		8
#define	GC_ALLOC_FIELD		9
#define	GC_ALLOC_STATICDATA	10
#define	GC_ALLOC_DISPATCHTABLE	11
#define	GC_ALLOC_BYTECODE	12
#define	GC_ALLOC_EXCEPTIONTABLE	13
#define	GC_ALLOC_CONSTANT	14
#define	GC_ALLOC_UTF8CONST	15
#define	GC_ALLOC_INTERFACE	16
#define	GC_ALLOC_JITCODE	17
#define	GC_ALLOC_LOCK		18
#define	GC_ALLOC_THREADCTX	19
#define	GC_ALLOC_REF		20

typedef struct GarbageCollectorInterface {

	void*	(*_malloc)(size_t, int);
	void*	(*_realloc)(void*, size_t, int);
	void	(*_free)(void*);

	void	(*_invokeGC)(void);
	void	(*_invokeFinalizer)(void);

	void	(*_addref)(void*);
	bool	(*_rmref)(void*);

} GarbageCollectorInterface;

extern GarbageCollectorInterface Kaffe_GarbageCollectorInterface;

#define	gc_malloc(A,B)		(*Kaffe_GarbageCollectorInterface._malloc)(A,B)
#define	gc_calloc(A,B,C)	(*Kaffe_GarbageCollectorInterface._malloc)((A)*(B),C)
#define	gc_calloc_fixed(A,B)	(*Kaffe_GarbageCollectorInterface._malloc)((A)*(B),GC_ALLOC_FIXED)
#define	gc_malloc_fixed(A)	(*Kaffe_GarbageCollectorInterface._malloc)(A,GC_ALLOC_FIXED)
#define	gc_realloc_fixed(A,B)	(*Kaffe_GarbageCollectorInterface._realloc)(A,B,GC_ALLOC_FIXED)
#define	gc_free(A)		(*Kaffe_GarbageCollectorInterface._free)(A)
#define	gc_free_fixed(A)	(*Kaffe_GarbageCollectorInterface._free)(A)

#define	invokeGC()		(*Kaffe_GarbageCollectorInterface._invokeGC)()
#define	invokeFinalizer()	(*Kaffe_GarbageCollectorInterface._invokeFinalizer)()

#define	gc_add_ref(A)		(*Kaffe_GarbageCollectorInterface._addref)(A)
#define	gc_rm_ref(A)		(*Kaffe_GarbageCollectorInterface._rmref)(A)

#endif
