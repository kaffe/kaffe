/*
 * jmalloc.h
 * Define the jmalloc/jfree functions
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __jmalloc_h
#define __jmalloc_h

#include <stdlib.h>
 
/*
 * Kaffe GC-aware replacements for malloc() and free(). Note that
 * there's no jcalloc() because jmalloc() returns zero'd memory.
 */

extern void*	jmalloc(size_t bytes);
extern void*	jrealloc(void* ptr, size_t bytes);
extern void	jfree(void* ptr);

#define KMALLOC(A)	jmalloc(A)
#define KREALLOC(A, B)	jrealloc((A), (B))
#define KCALLOC(A, B)	jmalloc((A) * (B))
#define KFREE(p)	jfree((void *)(p))

#ifdef KAFFE_VMDEBUG
/* Clear pointers after freeing the data, 
 * to aid debugging.
 * */
#undef KFREE
static inline void KFREE(void *p) {
	jfree (p);
	p = NULL;
}
#endif

#endif
