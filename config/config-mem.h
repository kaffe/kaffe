/*
 * config-mem.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __config_mem_h
#define __config_mem_h

#if defined(HAVE_STRING_H)
#include <string.h>
#endif

#if defined(HAVE_STDLIB_H)
# include <stdlib.h>
#else /* !HAVE_STDLIB_H */
# if defined(HAVE_MALLOC_H)
#  include <malloc.h>
# endif /* HAVE_MALLOC_H */
#endif /* HAVE_STDLIB_H */

#if defined(HAVE_ALLOCA_H)
#include <alloca.h>
#endif

#if defined(HAVE_MEMORY_H)
#include <memory.h>
#endif

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif /* defined(HAVE_SYS_TYPES_H) */

#if defined(HAVE_SYS_MMAN_H)
#include <sys/mman.h>
#endif /* defined(HAVE_SYS_MMAN_H) */

#if !defined(HAVE_MEMCPY)
void bcopy(void*, void*, size_t);
#define	memcpy(_d, _s, _n)	bcopy((_s), (_d), (_n))
#endif

#if !defined(HAVE_MEMMOVE)
/* use bcopy instead */
#define memmove(to,from,size)	bcopy((from),(to),(size))
#endif

#if !defined(HAVE_GETPAGESIZE)
#define	getpagesize()	8192
#endif

#if !defined(HAVE_DECLARED_SWAB)

/*
 * Linux 2.0 systems apparently put ssize_t in sys/types.h
 */
#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif

/* 
 * Some systems (Linux) do not declare swab in any standard header file
 */
/*
extern void swab(const void *from, void *to, size_t n);
*/
extern void swab(const void *from, void *to, ssize_t n);
#endif

#endif
