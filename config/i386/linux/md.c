/*
 * i386/linux/md.c
 * Linux i386 specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include <malloc.h>
#if defined(HAVE_FEATURES_H)
#include <features.h>
#endif

void
init_md(void)
{
#if defined(M_MMAP_MAX) && defined(HAVE_MALLOPT)
	mallopt(M_MMAP_MAX, 0);
#endif
}

#if defined(__GLIBC__)
extern void * __libc_stack_end;

void *mdGetStackBase(void)
{
  return __libc_stack_end;
}
#endif
