/*
 * powerpc/linux/md.c
 * Linux PowerPC specific functions.
 *
 * Copyright (c) 1996, 1997, 2004
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "gtypes.h"
#include "md.h"

#include <malloc.h>

void
init_md(void)
{
#if defined(HAVE_MALLOPT)
	mallopt(M_MMAP_MAX, 0);
#endif
}

#if defined(__GLIBC__)
extern void * __libc_stack_end;

void *mdGetStackEnd(void)
{
  long sz = sysconf(_SC_PAGESIZE);
  
  return (void *)(((uintp)__libc_stack_end + sz - 1) & (-sz));
}
#endif
