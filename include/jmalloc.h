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

/*
 * Kaffe GC-aware replacements for malloc() and free(). Note that
 * there's no jcalloc() because jmalloc() returns zero'd memory.
 */

extern void*	jmalloc(size_t);
extern void*	jrealloc(void*, size_t);
extern void	jfree(void*);

#define KMALLOC(A)	jmalloc(A)
#define KREALLOC(A, B)	jrealloc((A), (B))
#define KCALLOC(A, B)	jmalloc((A) * (B))
#define KFREE(p)	jfree(p)

#ifdef DEBUG
#undef KFREE
#define KFREE(p)	do { jfree(p); (void*)(p) = (void*)0; } while (0)
#endif

#endif
