/*
 * defs.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __defs_h
#define __defs_h

#include "gccbuiltin.h"

#define	MAXNUMLEN	64
#define	MAXLIBPATH	1024

#if !defined(MAXNAMELEN)
#define	MAXNAMELEN	1024
#endif

#if !defined(MAXPATHLEN)
#define	MAXPATHLEN	1024
#endif

#ifdef __cplusplus
#  define BEGIN_C_DECLS extern "C" {
#  define END_C_DECLS   }
#else /* !__cplusplus */
#  define BEGIN_C_DECLS
#  define END_C_DECLS
#endif /* __cplusplus */

/* define a cross platform VA_LIST_COPY(destination, source).
 *
 * We can's simply assign va_lists, since they are not necessarily structs.
 * On s390-linux, they are arrays, for example.
 * So let's use whatever system method there is to copy va_lists, before
 * we try to copy them by assignment.
 */
#if defined(HAVE_VA_COPY)
/* use va_copy */
#  define VA_LIST_COPY(dest, src) (va_copy(dest,src))
#else /* ! HAVE_VA_COPY */

/* use __va_copy*/
#  if defined(HAVE__VA_COPY)
#    define VA_LIST_COPY(dest, src) (__va_copy(dest, src))
#  else /* ! HAVE_VA_COPY && ! HAVE__VA_COPY */

/* va_list is an array, use memcpy */
#    if defined(VA_LIST_IS_ARRAY)
#     define VA_LIST_COPY(dest, src) (memcpy (dest, src, sizeof (dest)))
#    else /* ! HAVE_VA_COPY && ! HAVE__VA_COPY && ! VA_LIST_IS_ARRAY */

/* use plain assignment, then */
#     define VA_LIST_COPY(dest, src) (dest = src)
#    endif /* VA_LIST_IS_ARRAY */
#  endif /* HAVE__VA_COPY */
#endif /* HAVE_VA_COPY */

#ifndef NULL
#define NULL (void *)0
#endif

/* define alignment of 'void *' */
#ifndef ALIGNMENTOF_VOIDP
typedef struct { char c; void *p; } alignmentof_voidp_helper;
#define ALIGNMENTOF_VOIDP (offsetof(alignmentof_voidp_helper, p))
#endif

#endif /* __defs_h */
