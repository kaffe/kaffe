/*
 * jmalloc.h
 * Redirect the malloc/free functions
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __jmalloc_h
#define __jmalloc_h

#if !defined(KAFFEH)

extern void*	__malloc(size_t);
extern void	__free(void*);

#undef malloc
#undef calloc
#undef free

#define	malloc(A)	__malloc(A)
#define	calloc(A, B)	__malloc((A)*(B))
#define	free(A)		__free(A)

#endif

#endif
