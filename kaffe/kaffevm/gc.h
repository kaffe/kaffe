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

/* 
 * We do not support incremental collection as this time.
 */
#undef	GC_INCREMENTAL

/* Use the incremental garbage collector */
#include "mem/gc-incremental.h"

/*
 * Garbage collector interface.
 */
#define	GC_ALLOC_NORMAL		0
#define	GC_ALLOC_NOWALK		1
#define	GC_ALLOC_NORMALOBJECT	2
#define	GC_ALLOC_PRIMARRAY	3
#define	GC_ALLOC_REFARRAY	4
#define	GC_ALLOC_CLASSOBJECT	5
#define	GC_ALLOC_FINALIZEOBJECT	6
#define	GC_ALLOC_BYTECODE	7
#define	GC_ALLOC_EXCEPTIONTABLE	8
#define	GC_ALLOC_JITCODE	9
#define	GC_ALLOC_STATICDATA	10
#define	GC_ALLOC_CONSTANT	11
#define	GC_ALLOC_FIXED		12
#define	GC_ALLOC_DISPATCHTABLE	13
#define	GC_ALLOC_METHOD		14
#define	GC_ALLOC_FIELD		15
#define	GC_ALLOC_UTF8CONST	16
#define	GC_ALLOC_INTERFACE	17
#define	GC_ALLOC_LOCK		18
#define	GC_ALLOC_THREADCTX	19
#define	GC_ALLOC_REF		20

typedef struct GarbageCollectorInterface {

	void*	(*_malloc)(size_t, int);
	void*	(*_realloc)(void*, size_t, int);
	void	(*_free)(void*);

	void	(*_invokeGC)(void);
	void	(*_invokeFinalizer)(void);

	void	(*_addref)(const void*);
	bool	(*_rmref)(const void*);

} GarbageCollectorInterface;

extern GarbageCollectorInterface Kaffe_GarbageCollectorInterface;

#define	gc_malloc(A,B)		(*Kaffe_GarbageCollectorInterface._malloc)(A,B)
#define	gc_calloc(A,B,C)	(*Kaffe_GarbageCollectorInterface._malloc)((A)*(B),C)
#define	gc_free(A)		(*Kaffe_GarbageCollectorInterface._free)(A)

#define	invokeGC()		(*Kaffe_GarbageCollectorInterface._invokeGC)()
#define	invokeFinalizer()	(*Kaffe_GarbageCollectorInterface._invokeFinalizer)()

#define	gc_add_ref(A)		(*Kaffe_GarbageCollectorInterface._addref)(A)
#define	gc_rm_ref(A)		(*Kaffe_GarbageCollectorInterface._rmref)(A)

#endif
