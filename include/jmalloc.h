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

extern void*	jmalloc(size_t);
extern void	jfree(void*);

#undef malloc
#undef calloc
#undef free

#define	malloc(A)	jmalloc(A)
#define	calloc(A, B)	jmalloc((A)*(B))
#define	free(A)		jfree(A)

#endif

#endif
