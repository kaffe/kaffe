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
#if defined(HAVE_MALLOC_H)
#include <malloc.h>
#endif
#if defined(HAVE_ALLOCA_H)
#include <alloca.h>
#endif
#if defined(HAVE_MEMORY_H)
#include <memory.h>
#endif
#if defined(HAVE_MMAP)
#include <sys/mman.h>
#endif

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

#endif
